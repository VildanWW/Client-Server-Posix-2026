#include <iostream>
#include <exception>
#include "Server.h"

int main() {
    
    try {

        Server server;
        server.startServer();

        std::string command;
        while (true) {
            std::cin >> command;
            if (command == "stop") {
                break;
            }  
        } 
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << '\n';
    }
    std::cout << "Server worked successfully!\n";
    return 0;
}