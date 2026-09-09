#pragma once
#include <unordered_map>
#include <memory>
#include <thread>
#include "UserSession.h"
#include <mutex>
#include <condition_variable>

class DatabaseManager;

class Server {
private:
	std::unordered_map<int, std::unique_ptr<UserSession>> clientSessions;
	std::mutex serverMutexForSession;
	std::thread threadForListenning;
	std::thread threadForDeadSession;
	std::mutex serverMutexForTimer;
	std::condition_variable cvForTimer;

	int socketFd = -1;
	bool running = false;

	void cleanDeadSessions();
	void sendData(int socketFd, const InAppMessage& messageForServer);
	void timerCleanDeadSession();
	bool initializeSocketFd();
	bool stop();
	bool startListenning();
public:
	bool startServer();

	~Server();
};

