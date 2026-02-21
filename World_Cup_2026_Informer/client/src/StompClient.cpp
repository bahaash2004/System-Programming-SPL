#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include <thread>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    // --- שורת בדיקה: הדפסה שהתוכנית התחילה ---
    cout << "StompClient is running! Waiting for command..." << endl; 
    // ---------------------------------------------------

    ConnectionHandler* connectionHandler = nullptr;
    std::thread* socketThread = nullptr;
    StompProtocol protocol;
    bool isConnected = false;

    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        string line(buf);

        if (line.empty()) continue;

        vector<string> framesToSend = protocol.processUserInput(line);

        if (line.find("login") == 0) {
            if (isConnected) {
                cout << "The client is already logged in, log out before trying again" << endl;
                continue;
            }
            if (framesToSend.empty()) continue;

            string hostPort = line.substr(6, line.find(' ', 6) - 6);
            size_t colon = hostPort.find(':');
            string host = hostPort.substr(0, colon);
            short port = (short)stoi(hostPort.substr(colon + 1));

            connectionHandler = new ConnectionHandler(host, port);
            if (!connectionHandler->connect()) {
               std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                 delete connectionHandler;
                 connectionHandler = nullptr;
                continue;
            }

            isConnected = true;
            
            // יצירת התהליכון שמאזין לשרת
            socketThread = new std::thread([&connectionHandler, &protocol, &isConnected]() {
                while (isConnected) {
                    string answer;
                    if (!connectionHandler->getFrameAscii(answer, '\0')) {
                        cout << "Disconnected from server (Socket closed)." << endl;
                        isConnected = false;
                        protocol.setShouldTerminate(true);
                        break;
                    }
                    if (!protocol.processServerFrame(answer)) {
                        isConnected = false;
                        connectionHandler->close();
                    }
                }
            });
        }

        if (isConnected && connectionHandler) {
            for (const string& frame : framesToSend) {
                if (!connectionHandler->sendBytes(frame.c_str(), frame.length() + 1)) {
                    cout << "Error sending frame to server." << endl;
                    isConnected = false;
                    break;
                }
                // הדפסה למסך כדי שתראה שנשלח משהו
                if (line.find("login") == 0) cout << "Sent CONNECT frame to server" << endl;
            }
        } else if (!framesToSend.empty()) {
             cout << "Please login first." << endl;
        }

        if (protocol.shouldTerminate()) {
            if (socketThread && socketThread->joinable()) {
                socketThread->join();
                delete socketThread;
                socketThread = nullptr;
            }
            if (connectionHandler) {
                delete connectionHandler;
                connectionHandler = nullptr;
            }
            isConnected = false;
            protocol.setShouldTerminate(false);
        }
    }
    return 0;
}