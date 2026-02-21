#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <mutex> 
#include "../include/event.h"

class StompProtocol
{
private:
    std::mutex protocolMutex; // המנעול שמגן על המשתנים המשותפים
    int subscriptionIdCounter;
    int receiptIdCounter;
    std::string currentUser;
    bool shouldTerminate_flag;

    // Maps game name (topic) to subscription ID.
    std::map<std::string, int> topicToSubId;

    // Maps receipt ID to the pending action message.
    std::map<int, std::string> receiptIdToAction;

    // Map: Game Name -> (User Name -> List of Events)
    std::map<std::string, std::map<std::string, std::vector<Event>>> gameEvents;

public:
    StompProtocol();

    // Processes keyboard input from the user.
    std::vector<std::string> processUserInput(std::string line);

    // Processes a STOMP frame received from the server.
    bool processServerFrame(std::string& frame); // שים לב: העברתי ל-reference למניעת העתקות

    // Helpers
    bool shouldTerminate();
    void setShouldTerminate(bool val);
};