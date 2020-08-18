#include "stdafx.h"
#include "CommenHeaders.h"
#include "Server.h"
#include "FileUtils.h"
#include "ScopedThread.h"
#include "MsgInterpreter.h"
#include "Constants.h"

using std::vector;
using std::string;
using std::ifstream;
using std::pair;
SOCKET Server::servsocket = 0;
int Server::connection[1024]{};
Server::Server(int p_):port(p_) {							// Server���캯��

}

void Server::start() {										// �����ʼ���û���Ϣ��TCP����
	for (int i = 0; i < 1024; i++) {
		Server::connection[i] = -1;
	}
	vector<string> fileNames;
	try {
		getFileNames(ROOT_FILE, fileNames);					// ��ȡ�ļ����������ļ�
	}
	catch (char *errMsg) {
		std::cout << errMsg << std::endl;
		exit(-1);
	}
	// ���ļ������û��������ļ����ݣ��û����룩�����������map��
	for (auto it = fileNames.begin(); it != fileNames.end(); it++) {
		string path = ROOT_FILE + (std::string)"\\" + (*it);
		ifstream filename(path, std::ios::in);
		char buf[80];
		if (!filename.is_open()) {
			std::cout << "File doesn't exists" << std::endl;
			exit(-1);
		}
		filename.getline(buf, 80);
		(this->members).insert(pair<string, string>(*it, buf));
	}
	Server::servsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  
	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(this->port);
	//std::cout << this->port << std::endl;
	if (bind(Server::servsocket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1) {
		std::cout << "Fail to bind!" << std::endl;
	}
	if (listen(Server::servsocket, 20) == -1) {
		std::cout << "Fail to listen!" << std::endl;
	}
	ScopedThread st1(std::thread(& Server::connectionThread, std::ref(*this)));		// ���տͻ������ӵ��߳�
	ScopedThread st2(std::thread(& Server::businessThread, std::ref(*this)));		// ������տͻ�����Ϣ����ͻ��˷�����Ϣ	
}

// ���տͻ������ӵ��߳�
void Server::connectionThread(Server & s) {
	SOCKADDR clnaddr;
	int size = sizeof(SOCKADDR);
	for (;;) {
		auto s_c = accept(Server::servsocket, (SOCKADDR*)&clnaddr, &size);		// ��ѭ�����ȴ��ͻ�������
		std::cout << "connection received" << std::endl;
		int i;
		for (i = 0; i < 1024; i++) {				// ��socket������������
			if (Server::connection[i] == -1) {
				Server::connection[i] = s_c;
				break;
			}
		}
	}
}

// ����ҵ���߳�
void Server::businessThread(Server & s) {	
	fd_set scanfd;
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int err;
	string content;
	MESSAGE msg;
	char buffer[80];
	int hasMember;
	for (;;) {						// ����select����ѭ��������������Ƿ�����Ϣ����
		FD_ZERO(&scanfd);
		for (int i = 0; i < 1024; i++) {
			if (Server::connection[i] != -1) {
				FD_SET(Server::connection[i], &scanfd);
			}
		}
		err = select(0, &scanfd, nullptr, nullptr, &timeout);
		switch (err) {
		case 0:		// �ͻ���δ��������
			break;
		case -1:	// û�пɶ�ȡ��socket����
			break;
		default:	// �������յ��ͻ��˵���Ϣ
			for (int i = 0; i < 1024; i++) {
				if(Server::connection[i]!=-1)
					if (FD_ISSET(Server::connection[i], &scanfd)) {
						memset(buffer, 0, 80);
						if (recv(Server::connection[i], buffer, 80, 0) == 0) {
							closesocket(Server::connection[i]);
							Server::connection[i] = -1;
							continue;
						}
						content.assign(buffer);
						msg = interprete(content, 0);
						switch (msg.type) {
						case 1:						// ����ͻ��˵�¼��Ϣ
							int errtype;
							// ��½�ɹ�
							if (s.verify(msg.subject, msg.content, errtype)) {
								s.addSocketID(msg.subject, Server::connection[i]);
								content = "1" + msg.subject + SPLITTER + msg.object + SPLITTER + LOGIN_SUCCESS;
								std::queue<string> msgQueue = s.getMsgByUser(msg.subject);
								
								while (!msgQueue.empty()) {		// ��֮ǰ�������û�����Ϣ����
									string tmp;
									tmp = msgQueue.front();
									//std::cout << "tmp is" << tmp << std::endl;
									content = content + SPLITTER + "2" + tmp;
									msgQueue.pop();
								}
								s.removeMsg(msg.subject);
								send(Server::connection[i], content.c_str(), content.size(), 0);
							}
							// ��½ʧ��
							else {
								switch (errtype) {
								case 1:
									content = "1" + msg.subject + SPLITTER + msg.object + SPLITTER + NO_SUCH_USER;
									break;
								case 2:
									content = "1" + msg.subject + SPLITTER + msg.object + SPLITTER + WRONG_PASSWORD;
									break;
								default:
									content = "1" + msg.subject + SPLITTER + msg.object + SPLITTER + UNKNOWN_FAULT;
									break;
								}
								send(Server::connection[i], content.c_str(), content.size(), 0);
								closesocket(Server::connection[i]);
								Server::connection[i] = -1;
							}
							break;
						case 2:		// ��������ҵ��
							s.verify(msg.object, "", hasMember);
							if (hasMember == 1) {
								content = "2" + msg.subject + SPLITTER + HOST + SPLITTER + USER_NOT_EXIST;
								send(Server::connection[i], content.c_str(), content.size(), 0);
							}
							else {
								SOCKET sock = s.getSocketIDByName(msg.object);
								if (sock == -1) {
									s.addNewMsg(msg.object, msg.object + SPLITTER + msg.subject + SPLITTER + msg.content);
								}
								else {
									send(sock, content.c_str(), content.size(), 0);
								}
								//content = "";
							}
							break;
						case 3:		// �����˳�ҵ��
							s.removeSocketID(msg.subject);
							closesocket(Server::connection[i]);
							Server::connection[i] = -1;
							break;
						case 4:		// ����ע��ҵ��
							s.verify(msg.subject, "", hasMember);
							
							if (hasMember != 1 || msg.content == "") {
								content = "4" + msg.subject + SPLITTER + msg.object + SPLITTER + REGIST_FAIL;
							}
							else {
								creatFile((std::string)ROOT_FILE + "\\" + msg.subject, msg.content);
								s.addMember(msg.subject, msg.content);
								s.addSocketID(msg.subject, Server::connection[i]);
								content = "4" + msg.subject + SPLITTER + msg.object + SPLITTER + REGIST_SUCCESS;
 							}
							send(Server::connection[i], content.c_str(), content.size(), 0);
							break;
						}
					}
			}
		}
	}
}

// ͨ���û�����ȡ��Ӧ��socket����
SOCKET Server::getSocketIDByName(const std::string & userName) const{
	auto it = (this->userToSocketID).find(userName);
	if (it != (this->userToSocketID).end()) {
		return it->second;
	}
	else {
		return -1;
	}
}

// ����������µ�socket����
void Server::addSocketID(const std::string & userName, SOCKET s) {
	(this->userToSocketID).insert(pair<string, SOCKET>(userName, s));
}

// �������Ƴ����е�socket����
void Server::removeSocketID(const std::string & userName) {
	auto it = (this->userToSocketID).find(userName);
	if (it != (this->userToSocketID).end()) {
		(this->userToSocketID).erase(it);
	}
}

// ����ĳ���û�δ�յ�����Ϣ
std::queue<std::string> Server::getMsgByUser(const std::string & userName) const {
	auto it = (this->msgOfUser).find(userName);
	if (it != (this->msgOfUser).end()) {
		return (it->second);
	}
	return std::queue<std::string>();
}

// �û�������ʱ�������Ϣ�����û��Ļ��������
bool Server::addNewMsg(const std::string & userName, const std::string & msg) {
	auto it = (this->msgOfUser).find(userName);
	if (it == (this->msgOfUser).end()) {
		std::queue<string> msgQueue;
		msgQueue.push(msg);
		(this->msgOfUser).insert(pair<string, std::queue<string>>(userName, msgQueue));
		return false;
	}
	(it->second).push(msg);
	return true;
}

// �Ƴ�ĳ���û�
void Server::removeMsg(const std::string & userName) {
	auto it = (this->msgOfUser).find(userName);
	if (it != (this->msgOfUser).end()) {
		(this->msgOfUser).erase(it);
		return;
	}
	else {
		return;
	}
}

// ��֤�û���¼��Ϣ
bool Server::verify(const std::string & userName, const std::string & passWord, int & errtype) const {
	auto it = (this->members).find(userName);
	if (it == (this->members).end()) {
		errtype = 1;
		return false;
	}
	if (passWord != (it->second)) {
		errtype = 2;
		return false;
	}
	errtype = 0;
	return true;
}

// ����µ��û�
void Server::addMember(const std::string & userName, const std::string & passWord) {
	(this->members).insert(pair<std::string, std::string>(userName, passWord));
}


