package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.Database;
import bgu.spl.net.impl.data.LoginStatus;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;

import java.util.HashMap;
import java.util.Map;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate = false;

    // data for online client 
    private boolean loggedIn = false;
    private String currentUser = null;

    //  נצטרך את זה כדי לדעת לאיזה ערוץ לטבל רישום  בUNSUBSCRIBE
    private Map<String, String> subscriptions = new HashMap<>(); // topic -> subscriptionId


    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

 @Override
public void process(String message) {
    // שלב 1: הפרדה ראשונית בין Headers ל-Body לפי השורה הריקה הראשונה ("\n\n")
    String headersPart = message;
    String body = "";

    int splitIdx = message.indexOf("\n\n");
    if (splitIdx != -1) {
        // החלק הראשון הוא הפקודה והכותרות
        headersPart = message.substring(0, splitIdx);
        
        // החלק השני הוא הגוף (מדלגים על 2 תווים של \n\n)
        if (splitIdx + 2 < message.length()) {
            body = message.substring(splitIdx + 2);
        }
    }

    // שלב 2: ניתוח הפקודה והכותרות (עכשיו בטוח לעשות split כי אין פה body)
    String[] lines = headersPart.split("\n");
    if (lines.length == 0) return;

    // השורה הראשונה היא הפקודה
    String command = lines[0].trim();

    // חילוץ הכותרות (מתחילים משורה 1)
    Map<String, String> headers = new HashMap<>();
    for (int i = 1; i < lines.length; i++) {
        String[] parts = lines[i].split(":", 2);
        if (parts.length == 2) {
            headers.put(parts[0].trim(), parts[1].trim());
        }
    }

    // שלב 3: בדיקת התחברות
    if (!loggedIn && !command.equals("CONNECT")) {
        sendError("User not logged in", "You must log in first");
        return;
    }

    // שלב 4: ניתוב לפונקציות הטיפול
    switch (command) {
        case "CONNECT":
            handleConnect(headers);
            break;
        case "SEND":
            handleSend(headers, body); // מעבירים את ה-Body השלם והנכון
            break;
        case "SUBSCRIBE":
            handleSubscribe(headers);
            break;
        case "UNSUBSCRIBE":
            handleUnsubscribe(headers);
            break;
        case "DISCONNECT":
            handleDisconnect(headers);
            break;
        default:
            sendError("Unknown Command", "The command " + command + " is not recognized");
    }
}

    @Override
    public boolean shouldTerminate() {
       return shouldTerminate;
    }

    //------- Implementations of command handlers (CONNECT, SEND, SUBSCRIBE, UNSUBSCRIBE, DISCONNECT) ------


    private void handleConnect(Map<String, String> headers) {
    String login = headers.get("login");
    String passcode = headers.get("passcode");
    String version = headers.get("accept-version");
    //String host = headers.get("host");

    if (version == null || !version.equals("1.2")) {
        sendError("Version not supported", "Supported version is 1.2");
        return;
    }

    System.out.println("Login request received for user: " + login); //לצורך הדפסה בטמינל (סתם שיעזור לנו בבדיקות)
    LoginStatus status = Database.getInstance().login(connectionId,login, passcode);

    switch (status) {
        case LOGGED_IN_SUCCESSFULLY:
        case ADDED_NEW_USER:
            //התחברות הצליחה
            loggedIn = true;
            currentUser = login;
            String response = "CONNECTED\n" +
                              "version:1.2\n" +
                              "\n" ;
            connections.send(connectionId, response);
            break;

        case WRONG_PASSWORD:
            sendError("Login failed", "Wrong password");
            break;
        case ALREADY_LOGGED_IN:
            sendError("Login failed", "User already logged in");
            break;
        case CLIENT_ALREADY_CONNECTED:
            sendError("Login failed", "The current client is already logged in");
            break;
       
    }

}

    private void handleSend(Map<String, String> headers, String body) {
    // 1. חילוץ היעד (Destination)
    String destination = headers.get("destination");
    if (destination == null) {
        sendError("Malformed Frame", "Missing destination header");
        return;
    }

    // 2. בדיקת הרשאה (אופציונלי אך מומלץ)
    if (!subscriptions.containsValue(destination)) {
        sendError("Access Denied", "You must be subscribed to the topic to send messages");
        return;
    }

    // 3. יצירת Message-ID ייחודי
    String messageId = connectionId + "_" + System.currentTimeMillis();

    
    String senderUser = this.currentUser; 
        
        if (senderUser == null) {
             // זה לא אמור לקרות אם המשתמש עשה לוגין, אבל לביטחון:
             senderUser = headers.getOrDefault("user", "unknown");
        }
        // ---------------------

        String msgFrame = "MESSAGE\n" +
                         "destination:" + destination + "\n" +
                         "message-id:" + System.currentTimeMillis() + "\n" +
                         "subscription:" + "0" + "\n" + 
                         "user:" + senderUser + "\n" +  // עכשיו זה יהיה Alice בוודאות
                         "\n" +
                         body;

    // 5. הפצה לכל המנויים
    connections.send(destination, msgFrame);
    
    // 6. טיפול בקבלה (Receipt) לשולח
    if (headers.containsKey("receipt")) {
        sendReceipt(headers.get("receipt"));
    }
}

    private void handleSubscribe(Map<String, String> headers) {
        String destination = headers.get("destination");
        String id = headers.get("id");
        if (destination == null || id == null) {
            sendError("Missing headers", "SUBSCRIBE frame must have destination and id headers");
            return;
        }

        //שמירה מקומית של מינוי
        subscriptions.put(id, destination);
        if (connections instanceof ConnectionsImpl) {
            ((ConnectionsImpl<String>)connections).subscribe(destination, connectionId);
        }


        String receipt = headers.get("receipt");
        if (receipt != null) {
            sendReceipt(receipt);
        }
    }

    private void handleUnsubscribe(Map<String, String> headers) {
        String id = headers.get("id");
        if (id == null) {
            sendError("Malformed frame", "Missing id header");
            return;
        }
        String destination = subscriptions.remove(id);
        if (destination != null) {
            if(connections instanceof ConnectionsImpl){
                ((ConnectionsImpl<String>)connections).unsubscribe(destination, connectionId);
            }
        }

        String receipt = headers.get("receipt");
        if (receipt != null) {
            sendReceipt(receipt);
        }
       
    }

    private void handleDisconnect(Map<String, String> headers) {
        String receipt = headers.get("receipt");
        if (receipt != null) {
           sendReceipt(receipt);
        }

        if(loggedIn){
            Database.getInstance().logout(connectionId);
        }
        loggedIn = false;
        shouldTerminate = true;
    }

    //--- helper methods ----

   private void sendError(String errorMessage, String details) {
    String errorMsg = "ERROR\n" +
                      "message:" + errorMessage + "\n" +
                      "\n" +
                      details; 
                      
    connections.send(connectionId, errorMsg);
    shouldTerminate = true;
}

    private void sendReceipt(String receiptId) {
        String frame = "RECEIPT\n" +
                       "receipt-id:" + receiptId + "\n" +
                       "\n" ;
    }


}