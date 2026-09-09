#pragma once
#include <string>
#include <thread>
#include <functional>
#include <vector>
#include "Settings.h"
#include <chrono>

class UserSession {
private:
	std::function<void(int, const InAppMessage&)> onMessageReceived;

	std::thread clientThread;
	std::string name;

	UserStatus status = UserStatus::WAITING_NAME;

	int socketFd;
	bool running = false;

	void runWorking();
	std::string generateUniqueKey(const std::string& userName, int socketFd, DataType type);
	bool stop();
public:
	UserSession(int socketFd, 
		std::function<void(int, const InAppMessage&)> onMessageReceived)
		: name(""), socketFd(socketFd), onMessageReceived(onMessageReceived) {}

	bool getRunning();
	bool startWorking();

	~UserSession();
};