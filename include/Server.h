#pragma once
#ifndef SERVER_H__
#define SERVER_H__
#include "CommenHeaders.h"

class Server {
private:
	int port;
	std::map<std::string, SOCKET> userToSocketID;
	std::unordered_map<std::string, std::queue<std::string>> msgOfUser;
	std::map<std::string, std::string> members;
public:
	explicit Server(int p_=8888);
	virtual ~Server() = default;
	SOCKET getSocketIDByName(const std::string &) const;
	void addSocketID(const std::string &, SOCKET);
	void removeSocketID(const std::string &);
	std::queue<std::string> getMsgByUser(const std::string &) const;
	bool addNewMsg(const std::string &, const std::string &);
	void removeMsg(const std::string &);
	bool verify(const std::string &, const std::string &, int &) const;
	void addMember(const std::string &, const std::string &);
	void start();
	Server(Server const&) = delete;
	Server(Server &&) = delete;
	Server& operator=(Server const&) = delete;
	static void connectionThread(Server &);
	static void businessThread(Server &);
	static SOCKET servsocket;
	static int connection[1024];
};

#endif;
