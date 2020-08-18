#pragma once
#include "CommenHeaders.h"
class Client {
private:
	enum {ONLINE, LOGIN, REGIST, USING, QUIT};
	int port;
	bool bridge;
	std::string userName;
	std::string recvContent;
	std::string sendContent;
public:
	explicit Client(int = 8888);
	virtual ~Client() = default;
	Client(Client const&) = delete;
	Client(Client &&) = delete;
	Client& operator=(Client const&) = delete;
	static void help();
	static void sendThread(Client &, SOCKET);
	static void recvThread(Client &, SOCKET);
	static void start(Client &);
};