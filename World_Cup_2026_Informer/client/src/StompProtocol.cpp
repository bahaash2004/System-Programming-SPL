#include "../include/StompProtocol.h"
#include "../include/event.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <mutex>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::stringstream;
using std::map;

// ==========================================
// פונקציית עזר: SPLIT
// חייבת להיות מוגדרת *לפני* שמשתמשים בה
// ==========================================
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ==========================================
// Constructor
// ==========================================
StompProtocol::StompProtocol() : 
    subscriptionIdCounter(0), 
    receiptIdCounter(0), 
    currentUser(""), 
    shouldTerminate_flag(false),
    topicToSubId(), 
    receiptIdToAction(),
    gameEvents() 
{}

bool StompProtocol::shouldTerminate() {
    return shouldTerminate_flag;
}

void StompProtocol::setShouldTerminate(bool val) {
    shouldTerminate_flag = val;
}

// ==========================================
// Process User Input (Keyboard)
// ==========================================
vector<string> StompProtocol::processUserInput(string line) {
    vector<string> framesToSend;
    stringstream ss(line);
    string command;
    ss >> command;

    // 1. LOGIN
    if (command == "login") {
        string hostPort, username, password;
        ss >> hostPort >> username >> password;
        
        if (username.empty() || password.empty()) {
            cout << "Error: Invalid login format." << endl;
            return framesToSend;
        }

        currentUser = username; 

        string frame = "CONNECT\n";
        frame += "accept-version:1.2\n";
        frame += "host:stomp.cs.bgu.ac.il\n";
        frame += "login:" + username + "\n";
        frame += "passcode:" + password + "\n";
        frame += "\n\0"; 

        framesToSend.push_back(frame);
    }

    // 2. JOIN (SUBSCRIBE)
    else if (command == "join") {
        string gameName;
        ss >> gameName;

        // 1. נעילה (כי אנחנו ניגשים למפה המשותפת)
        std::lock_guard<std::mutex> lock(protocolMutex);

        // 2. בדיקה: האם כבר נרשמנו למשחק הזה?
        if (topicToSubId.count(gameName)) {
            cout << "Already subscribed to channel " << gameName << endl;
            return {}; // מחזירים וקטור ריק -> לא שולחים כלום לשרת
        }
        
        int id = subscriptionIdCounter++;
        int receiptId = receiptIdCounter++;

        topicToSubId[gameName] = id;
        receiptIdToAction[receiptId] = "joined channel " + gameName;

        stringstream frame;
        frame << "SUBSCRIBE\n";
        frame << "destination:/" << gameName << "\n";
        frame << "id:" << id << "\n";
        frame << "receipt:" << receiptId << "\n\n"; // שים לב ל- \n\n

        framesToSend.push_back(frame.str());
    }

    // 3. EXIT (UNSUBSCRIBE)
    else if (command == "exit") {
        string gameName;
        ss >> gameName;

        std::lock_guard<std::mutex> lock(protocolMutex); // <--- נעילה

        if (topicToSubId.find(gameName) == topicToSubId.end()) {
            cout << "Error: You are not subscribed to " << gameName << endl;
            return framesToSend;
        }

        int subId = topicToSubId[gameName];
        receiptIdCounter++;
        int receiptId = receiptIdCounter;

        receiptIdToAction[receiptId] = "Exited channel " + gameName;

        string frame = "UNSUBSCRIBE\n";
        frame += "id:" + std::to_string(subId) + "\n";
        frame += "receipt:" + std::to_string(receiptId) + "\n";
        frame += "\n\0";

        framesToSend.push_back(frame);
        
        // מחיקה מהמפה תתבצע רק כשנקבל RECEIPT (או כאן, תלוי במימוש, כאן בחרנו לחכות או למחוק מיד. למען הפשטות מחקנו)
        topicToSubId.erase(gameName); 
    }

    // 4. REPORT (SEND)
    else if (command == "report") {
        string filePath;
        ss >> filePath;
        
        names_and_events parsedData;
        try {
            parsedData = parseEventsFile(filePath);
        } catch (...) {
            cout << "Error: Failed to parse file " << filePath << endl;
            return framesToSend;
        }

        string gameName = parsedData.team_a_name + "_" + parsedData.team_b_name;

        std::lock_guard<std::mutex> lock(protocolMutex);
        for (const Event& event : parsedData.events) {
            // Save locally
            gameEvents[gameName][currentUser].push_back(event);

            // Build SEND frame
            string frame = "SEND\n";
            frame += "destination:/" + gameName + "\n";
            frame += "\n"; 
            
            frame += "user: " + currentUser + "\n";
            frame += "team a: " + parsedData.team_a_name + "\n";
            frame += "team b: " + parsedData.team_b_name + "\n";
            frame += "event name: " + event.get_name() + "\n";
            frame += "time: " + std::to_string(event.get_time()) + "\n";
            
            frame += "general game updates:\n";
            for (auto const& [key, val] : event.get_game_updates()) {
                frame += key + ": " + val + "\n";
            }

            frame += "team a updates:\n";
            for (auto const& [key, val] : event.get_team_a_updates()) {
                frame += key + ": " + val + "\n";
            }

            frame += "team b updates:\n";
            for (auto const& [key, val] : event.get_team_b_updates()) {
                frame += key + ": " + val + "\n";
            }

            frame += "description:\n" + event.get_discription() + "\n";
            frame += "\0";

            framesToSend.push_back(frame);
        }
    }

    // 5. LOGOUT (DISCONNECT)
    else if (command == "logout") {
        std::lock_guard<std::mutex> lock(protocolMutex); // <--- נעילה
        receiptIdCounter++;
        int receiptId = receiptIdCounter;
        receiptIdToAction[receiptId] = "DISCONNECT";

        string frame = "DISCONNECT\n";
        frame += "receipt:" + std::to_string(receiptId) + "\n";
        frame += "\n\0";
        framesToSend.push_back(frame);
    }

    // 6. SUMMARY
    else if (command == "summary") {
        string gameName, user, file;
        ss >> gameName >> user >> file;

        // קריטי! חייבים לנעול כי ת'רד הרשת יכול לכתוב לפה באותו זמן
        std::lock_guard<std::mutex> lock(protocolMutex);
        if (gameEvents.find(gameName) == gameEvents.end() || 
            gameEvents[gameName].find(user) == gameEvents[gameName].end()) {
            cout << "Error: No events found for game " << gameName << " from user " << user << endl;
            return framesToSend;
        }

        vector<Event>& events = gameEvents[gameName][user];
        
        // Structures to hold the FINAL stats (last update wins)
        map<string, string> game_stats;
        map<string, string> team_a_stats;
        map<string, string> team_b_stats;

        // Iterate through all events to update stats
        for (const auto& ev : events) {
            for (auto const& [key, val] : ev.get_game_updates()) game_stats[key] = val;
            for (auto const& [key, val] : ev.get_team_a_updates()) team_a_stats[key] = val;
            for (auto const& [key, val] : ev.get_team_b_updates()) team_b_stats[key] = val;
        }

        // Get team names from the first event
        string teamA = events[0].get_team_a_name();
        string teamB = events[0].get_team_b_name();

        std::ofstream outfile(file);
        if (outfile.is_open()) {
             outfile << teamA << " vs " << teamB << endl;
             outfile << "Game stats:" << endl;
             outfile << "General stats:" << endl;
             for (auto const& [key, val] : game_stats) outfile << key << ": " << val << endl;
             
             outfile << teamA << " stats:" << endl;
             for (auto const& [key, val] : team_a_stats) outfile << key << ": " << val << endl;

             outfile << teamB << " stats:" << endl;
             for (auto const& [key, val] : team_b_stats) outfile << key << ": " << val << endl;

             outfile << "Game event reports:" << endl;
             for (const auto& ev : events) {
                 outfile << ev.get_time() << " - " << ev.get_name() << ":" << endl;
                 outfile << endl;
                 outfile << ev.get_discription() << endl << endl;
             }
             outfile.close();
             cout << "Summary created in " << file << endl;
        } else {
            cout << "Error: Could not open file " << file << endl;
        }
    }

    return framesToSend;
}

// ==========================================
// Process Server Frame
// ==========================================

bool StompProtocol::processServerFrame(string& answer) {
    // 1. הפרדה בין Headers ל-Body לפי השורה הריקה הראשונה
    string headersPart;
    string bodyPart;
    
    size_t splitPos = answer.find("\n\n");
    if (splitPos != string::npos) {
        headersPart = answer.substr(0, splitPos);
        // דילוג על 2 תווים של \n\n
        if (splitPos + 2 < answer.length()) {
            bodyPart = answer.substr(splitPos + 2);
        }
    } else {
        headersPart = answer; // הודעה ללא גוף (כמו CONNECTED או RECEIPT)
    }

    // 2. פירוק ה-Headers
    vector<string> headerLines = split(headersPart, '\n');
    if (headerLines.empty()) return false;

    string command = headerLines[0];
    std::map<string, string> headers;

    for (size_t i = 1; i < headerLines.size(); ++i) {
        size_t colonPos = headerLines[i].find(':');
        if (colonPos != string::npos) {
            string key = headerLines[i].substr(0, colonPos);
            string value = headerLines[i].substr(colonPos + 1);
            headers[key] = value;
        }
    }

    // טיפול ב-CONNECTED (התחברות מוצלחת)
    if (command == "CONNECTED") {
        cout << "Login successful" << endl;
        return true;
    }

    // טיפול ב-ERROR (שגיאה וניתוק)
    else if (command == "ERROR") {
        cout << "Error message received from server:" << endl;
        if (headers.count("message")) {
            cout << headers["message"] << endl;
        }
        cout << bodyPart << endl; // הדפסת הגוף (לרוב מכיל פרטים נוספים)
        shouldTerminate_flag = true;
        return false; // סגירת חיבור
    }

    // טיפול ב-MESSAGE (עדכון על משחק)
    else if (command == "MESSAGE") {
        string user = headers["user"];
        string gameName = headers["destination"];
        
        // משתנים לשמירת נתוני האירוע
        string event_name, team_a, team_b, description;
        int time = 0;
        std::map<string, string> general_updates, team_a_updates, team_b_updates;

        // פירוק הגוף לשורות כדי לנתח את התוכן
        vector<string> bodyLines = split(bodyPart, '\n');
        string current_section = "";

        for (const string& line : bodyLines) {
            if (current_section == "description") {
                description += line + "\n";
                continue;
            }

            if (line == "general game updates:") { current_section = "general"; continue; }
            if (line == "team a updates:") { current_section = "team_a"; continue; }
            if (line == "team b updates:") { current_section = "team_b"; continue; }
            if (line == "description:") { current_section = "description"; continue; }

            size_t colon = line.find(':');
            if (colon != string::npos) {
                string key = line.substr(0, colon);
                string val = line.substr(colon + 1);
                
                if (!val.empty() && val[0] == ' ') val = val.substr(1);

                if (key == "event name") event_name = val;
                else if (key == "time") {
                    try { time = stoi(val); } catch(...) { time = 0; }
                }
                else if (key == "team a") team_a = val;
                else if (key == "team b") team_b = val;
                else if (current_section == "general") general_updates[key] = val;
                else if (current_section == "team_a") team_a_updates[key] = val;
                else if (current_section == "team_b") team_b_updates[key] = val;
            }
        }

        // --- התיקון למניעת כפילויות ---
        if (user != currentUser) {
             if (!gameName.empty() && !user.empty()) {
                Event event(team_a, team_b, event_name, time, general_updates, team_a_updates, team_b_updates, description);
                std::lock_guard<std::mutex> lock(protocolMutex);
                gameEvents[gameName][user].push_back(event);
            }
        }
        
        cout << "Received Message from " << user << " (" << gameName << "): " << event_name << endl;
        return true;
    }

    // טיפול ב-RECEIPT (אישור פעולה)
    else if (command == "RECEIPT") {
        string receiptIdStr = headers["receipt-id"];
        int receiptId = stoi(receiptIdStr);

        std::lock_guard<std::mutex> lock(protocolMutex);

        if (receiptIdToAction.count(receiptId)) {
            string action = receiptIdToAction[receiptId];
            cout << "Completed action: " << action << endl;

            if (action == "DISCONNECT") {
                shouldTerminate_flag = true;
            }
            receiptIdToAction.erase(receiptId);
        }
        return true;
    }

    return true;
}