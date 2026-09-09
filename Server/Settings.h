#pragma once
#include <vector>

enum UserStatus : int32_t {
	WAITING_NAME,
	CONNECTED_TO_CHAT
};

enum class DataType : int32_t {
	TEXT,
	IMAGE,
	AUDIO,
	VIDEO
};

struct PacketData {
	DataType packetType;
	int dataSize;
	char senderName[32];
};

struct InAppMessage {
	DataType type;
	std::string senderName;      
	std::vector<char> dataBuffer;
};

struct ServerConfig {
	static constexpr int port = 9090;
	static constexpr int sizeLog = 64;
	static constexpr int bufferSize = 4096;
	static constexpr int timeCleanSession = 5;
	static constexpr int maxPacketSize = 10 * 1024 * 1024;

	static constexpr int dbPort = 5432;
	static constexpr const char* dbHost = "127.0.0.1";
	static constexpr const char* dbUser = "postgres";
	static constexpr const char* dbPass = "12345";
	static constexpr const char* dbName = "chat_db";
};