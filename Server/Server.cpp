#include "Server.h"
#include <thread>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "UserSession.h"
#include "Settings.h"
#include <cstring>

bool Server::startListenning() {
	while (running) {
		sockaddr_in clientAddress;
		socklen_t sizeAddr = sizeof(clientAddress);

		int clientSocketFd = accept(socketFd, (sockaddr*)&clientAddress, &sizeAddr);

		if (clientSocketFd < 0) {
			std::cerr << "ClientSocketFd is wrong\n";
			continue;
		}

		std::cout << "New good connect!\n";
		std::lock_guard<std::mutex> lockGuard(serverMutexForSession);
		{
			clientSessions[clientSocketFd] = std::make_unique<UserSession>( clientSocketFd, 
				[this](int fd, const InAppMessage& messageForServer) {
					this->sendData(fd, messageForServer);
				}
			);
		}

		clientSessions[clientSocketFd]->startWorking();
	}

	return true;
}

void Server::sendData(int socketFd, const InAppMessage& messageForServer) {
	{
		std::lock_guard<std::mutex> lockGuard(serverMutexForSession);
		for (const auto& client : clientSessions) {
			int clientFd = client.first;

			if (socketFd == clientFd || !client.second->getRunning()) continue;

			PacketData packet;
			memset(&packet, 0, sizeof(PacketData));

			packet.packetType = messageForServer.type;
			packet.dataSize = messageForServer.dataBuffer.size();

			size_t copiedBytes = messageForServer.senderName.copy(packet.senderName, sizeof(packet.senderName) - 1);
			packet.senderName[copiedBytes] = '\0';

			int bytesSentPacket = send(clientFd, &packet, sizeof(PacketData), 0);

			if (bytesSentPacket != sizeof(PacketData)) {
				std::cerr << "BytesSentPacket != sizeof(PacketData), client clientFd: " << clientFd << '\n';
				continue;
			}

			int bytesSentData = send(clientFd, messageForServer.dataBuffer.data(), messageForServer.dataBuffer.size(), 0);
			
			if (bytesSentData != static_cast<int>(messageForServer.dataBuffer.size())) {
				std::cerr << "bytesSentData != static_cast<int>(messageForServer.dataBuffer.size()), client clientFd: " << clientFd << '\n';
				continue;
			}
		}
	}
}

void Server::cleanDeadSessions() {
	std::lock_guard<std::mutex> lockGuard(serverMutexForSession);

	for (auto it = clientSessions.begin(); it != clientSessions.end();) {
		if (!it->second->getRunning()) {
			it = clientSessions.erase(it);
			std::cout << "Client session deleted from server memory\n";
		}
		else {
			it++;
		}
	}
}

void Server::timerCleanDeadSession() {
	std::unique_lock<std::mutex> lockGuard(serverMutexForTimer);
	while (running) {
		cvForTimer.wait_for(lockGuard, std::chrono::seconds(ServerConfig::timeCleanSession), [this]() {
			return !running;
		});

		if (!running) break;

		cleanDeadSessions();
	}
}

bool Server::startServer() {
	if (!initializeSocketFd()) {
		std::cerr << "Server can't to initialize the socketFd\n";
		return false;
	}

	running = true;

	threadForListenning = std::thread(&Server::startListenning, this);
	threadForDeadSession = std::thread(&Server::timerCleanDeadSession, this);

	std::cout << "Server is running!\n";
	return true;
}

bool Server::initializeSocketFd() {
	socketFd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFd < 0) {
		std::cerr << "Failed to create the socketFd\n";
		return false;
	}

	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(ServerConfig::port);

	socklen_t sizeAddr = sizeof(address);
	if (bind(socketFd, (sockaddr*)&address, sizeAddr) < 0){
		std::cerr << "Error with bind\n";
		close(socketFd);
		return false;
	}

	if (listen(socketFd, ServerConfig::sizeLog) < 0) {
		std::cerr << "Can't to tune listen with sizeLog\n";
		close(socketFd);
		return false;
	}
	
	std::cout << "Server is running on port: " << ServerConfig::port << '\n';
	
	return true;
}

bool Server::stop() {
	if (!running) return false;
	running = false;
	cvForTimer.notify_all();

	if (socketFd != -1) {
		shutdown(socketFd, SHUT_RDWR);
		close(socketFd);
		socketFd = -1;
	}
	std::cout << "Method stop of Server worked successfully\n";
	return true;
}

Server::~Server() {
	stop();
	if (threadForListenning.joinable()) {
		threadForListenning.join();
	}
	if (threadForDeadSession.joinable()) {
		threadForDeadSession.join();
	}
	std::cout << "Server destructor worked successfully\n";
}

