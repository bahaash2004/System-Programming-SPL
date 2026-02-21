package bgu.spl.net.impl.data;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

public class Database {
    private final ConcurrentHashMap<String, User> userMap;
    private final ConcurrentHashMap<Integer, User> connectionsIdMap;
    private final String sqlHost;
    private final int sqlPort;

    private Database() {
        userMap = new ConcurrentHashMap<>();
        connectionsIdMap = new ConcurrentHashMap<>();
        // כתובת שרת ה-SQL (פייתון)
        this.sqlHost = "127.0.0.1";
        this.sqlPort = 7778;
    }

    public static Database getInstance() {
        return Instance.instance;
    }

    /**
     * שליחת פקודת SQL לשרת הפייתון וקבלת תשובה
     */
    private String executeSQL(String sql) {
        // הדפסה לפני הניסיון להתחבר
        System.out.println("[DEBUG] Java attempting to connect to SQL Server at " + sqlHost + ":" + sqlPort);
        
        try (Socket socket = new Socket(sqlHost, sqlPort);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {
            
            System.out.println("[DEBUG] Connected! Sending SQL: " + sql);
            
            // שליחה עם תו מסיים
            out.print(sql + '\0');
            out.flush();
            
            // קריאה עד תו מסיים
            StringBuilder response = new StringBuilder();
            int ch;
            while ((ch = in.read()) != -1 && ch != '\0') {
                response.append((char) ch);
            }
            
            String res = response.toString();
            System.out.println("[DEBUG] SQL Response: " + res);
            return res;
            
        } catch (IOException e) {
            // הדפסה חזקה של השגיאה
            System.out.println("[CRITICAL ERROR] Failed to connect to Python SQL Server!");
            e.printStackTrace();
            return "ERROR:" + e.getMessage();
        }
    }

    /**
     * ניקוי תווים בעייתיים למניעת שגיאות SQL
     */
    private String escapeSql(String str) {
        if (str == null) return "";
        return str.replace("'", "''");
    }

    /**
     * טיפול בהתחברות (לוגין)
     */
    public LoginStatus login(int connectionId, String username, String password) {
        // 1. בדיקה אם הקליינט כבר מחובר עם משתמש אחר
        if (connectionsIdMap.containsKey(connectionId)) {
            return LoginStatus.CLIENT_ALREADY_CONNECTED;
        }

        // 2. טיפול במשתמש חדש או קיים
        // אנחנו מסתמכים על המפה בזיכרון כמקור האמת לסטטוס "מחובר כרגע"
        
        synchronized (userMap) {
            User user = userMap.get(username);
            
            // מקרה א': משתמש לא קיים בזיכרון (אולי חדש, אולי קיים ב-DB)
            if (user == null) {
                user = new User(connectionId, username, password);
                user.login();
                
                // מנסים לשמור ב-DB
                String sql = String.format(
                    "INSERT INTO users (username, password, registration_date) VALUES ('%s', '%s', datetime('now'))",
                    escapeSql(username), escapeSql(password)
                );
                
                String dbResponse = executeSQL(sql);
           
                
                userMap.put(username, user);
                connectionsIdMap.put(connectionId, user);
                
                logLogin(username); // תיעוד בהיסטוריה
                return LoginStatus.ADDED_NEW_USER;
            } 
            
            // מקרה ב': משתמש קיים בזיכרון
            else {
                if (user.isLoggedIn()) {
                    return LoginStatus.ALREADY_LOGGED_IN;
                } else if (!user.password.equals(password)) {
                    return LoginStatus.WRONG_PASSWORD;
                } else {
                    user.login();
                    user.setConnectionId(connectionId);
                    connectionsIdMap.put(connectionId, user);
                    
                    logLogin(username); // תיעוד בהיסטוריה
                    return LoginStatus.LOGGED_IN_SUCCESSFULLY;
                }
            }
        }
    }

    private void logLogin(String username) {
        String sql = String.format(
            "INSERT INTO login_history (username, login_time) VALUES ('%s', datetime('now'))",
            escapeSql(username)
        );
        executeSQL(sql);
    }

    public void logout(int connectionsId) {
        User user = connectionsIdMap.remove(connectionsId);
        if (user != null) {
            user.logout();
            
            // עדכון זמן יציאה ב-SQL
            String sql = String.format(
                "UPDATE login_history SET logout_time=datetime('now') " +
                "WHERE username='%s' AND logout_time IS NULL " +
                "ORDER BY id DESC LIMIT 1", // שימוש ב-id עדיף אם קיים, אחרת login_time
                escapeSql(user.name)
            );
            executeSQL(sql);
        }
    }
    
    /**
     * רישום העלאת קובץ (Report) ל-DB
     */
    public void trackFileUpload(String username, String filename, String gameChannel) {
        String sql = String.format(
            "INSERT INTO file_tracking (username, filename, upload_time, game_channel) " +
            "VALUES ('%s', '%s', datetime('now'), '%s')",
            escapeSql(username), escapeSql(filename), escapeSql(gameChannel)
        );
        executeSQL(sql);
    }

    /**
     * הדפסת דוח נתונים (נקרא ע"י פקודת שרת מיוחדת או בדיבוג)
     */
    public void printReport() {
        System.out.println(repeat("=", 80));
        System.out.println("SERVER REPORT");
        System.out.println(repeat("=", 80));
        
        // 1. Registered Users
        System.out.println("\n1. REGISTERED USERS:");
        String usersResult = executeSQL("SELECT username, registration_date FROM users ORDER BY registration_date");
        printRawResult(usersResult);
        
        // 2. Login History
        System.out.println("\n2. LOGIN HISTORY:");
        String loginResult = executeSQL("SELECT username, login_time, logout_time FROM login_history ORDER BY id DESC");
        printRawResult(loginResult);
        
        // 3. File Uploads
        System.out.println("\n3. FILE UPLOADS:");
        String filesResult = executeSQL("SELECT username, filename, upload_time, game_channel FROM file_tracking ORDER BY id DESC");
        printRawResult(filesResult);
        
        System.out.println(repeat("=", 80));
    }
    
    private void printRawResult(String result) {
        if (result.startsWith("SUCCESS")) {
            String[] parts = result.split("\\|");
            for (int i = 1; i < parts.length; i++) {
                System.out.println("   " + parts[i]);
            }
            if (parts.length == 1) System.out.println("   (No records found)");
        } else {
            System.out.println("   Query Error: " + result);
        }
    }

    private String repeat(String str, int times) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < times; i++) sb.append(str);
        return sb.toString();
    }

    private static class Instance {
        static Database instance = new Database();
    }
}