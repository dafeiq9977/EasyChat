#include "stdafx.h"
#include "CommenHeaders.h"
#include "Client.h"
#include "MsgInterpreter.h"
#include "ScopedThread.h"
#include "Constants.h"

// Client线程
Client::Client(int p_) :port(p_),bridge(false) {

}

// 负责客户端到服务器的连接和登录发信息注册等业务
void Client::start(Client & cln) {
	SOCKET sock;
	sockaddr_in sockAddr;
	int err;
	int status = Client::ONLINE;
	std::string userName;
	std::string passWord;
	std::string option;
	MESSAGE msg;
	int len = 0;
	char buffer[1024];
	memset(buffer, 0, 1024);
	// 循环处理各种业务
	while (status != Client::QUIT) {
		switch (status) {
		// 登录业务
		case Client::LOGIN:
			cln.bridge = false;
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			//向服务器发起请求
			memset(&sockAddr, 0, sizeof(sockAddr));
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_addr.s_addr = htonl(2130706433);
			sockAddr.sin_port = htons(5010);
			err = connect(sock, (SOCKADDR *)&sockAddr, sizeof(SOCKADDR));
			if (err == -1) {
				char erromsg[1024];
				strerror_s(erromsg, 1024, GetLastError());
				printf("%s\n", erromsg);
				exit(-1);
				std::cout << GetLastError() << std::endl;
			}
			std::cout << "请输入你的用户名：";
			std::cin >> userName;
			std::cout << "请输入你的密码：";
			std::cin >> passWord;
			std::cin.get();
			(cln.sendContent) = (std::string)"1Server" + SPLITTER + userName + SPLITTER + passWord;
			send(sock, (cln.sendContent).c_str(), (cln.sendContent).size(), 0);
			len = recv(sock, buffer, 1024, 0);
			(cln.recvContent).assign(buffer);
			// 解析服务器返回的登录信息
			while (len > 0) {
				msg = interprete(cln.recvContent, 0);
				int length = messageLength(msg);
				len -= length;
				(cln.recvContent).erase(0, length);
				std::cout << "From:" << msg.subject << "  " << msg.content << std::endl;
				if ((msg.content == LOGIN_SUCCESS)) {
					cln.userName = msg.object;
					status = Client::USING;
				}
			}
			std::cout.flush();
			break;
		// 功能选择界面
		case Client::ONLINE:
			std::cout << "请输入您的需求：" << std::endl;
			std::cout << "1. 登陆" << std::endl;
			std::cout << "2. 注册" << std::endl;
			std::cout << "3. 帮助" << std::endl;
			std::cout << "4. 退出" << std::endl;
			std::cin.getline(buffer, 1024);
			option.assign(buffer);
			// 切换客户端的状态
			if (option == "1") {
				status = Client::LOGIN;
			}
			else if (option == "2") {
				status = Client::REGIST;
			}
			else if (option == "3") {
				Client::help();
				status = Client::ONLINE;
			}
			else if (option == "4") {
				status = Client::QUIT;
			}
			else {
				std::cout << "输入有误，请选择相应的数字！" << std::endl;
			}
			break;
		// 注册业务
		case Client::REGIST:
			cln.bridge = false;
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			memset(&sockAddr, 0, sizeof(sockAddr)); 
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_addr.s_addr = htonl(2130706433);
			sockAddr.sin_port = htons(5010);
			err = connect(sock, (SOCKADDR *)&sockAddr, sizeof(SOCKADDR));
			if (err == -1) {
				char erromsg[1024];
				strerror_s(erromsg, 1024, GetLastError());
				printf("%s\n", erromsg);
				exit(-1);
				std::cout << GetLastError() << std::endl;
			}
			std::cout << "请输入注册的用户名：";
			std::cin >> userName;
			std::cout << "请输入您的密码：";
			std::cin >> passWord;
			std::cin.get();
			// 发送注册信息
			cln.sendContent = (std::string)"4" + HOST + SPLITTER + userName + SPLITTER + passWord;
			send(sock, (cln.sendContent).c_str(), (cln.sendContent).size(), 0);
			recv(sock, buffer, 1024, 0);
			(cln.recvContent).assign(buffer);
			msg = interprete(cln.recvContent, 0);
			std::cout << "From:" << msg.subject << " " << msg.content << std::endl;
			// 根据服务器反馈会的信息切换状态
			if (msg.content == REGIST_SUCCESS) {
				cln.userName = userName;
				status = Client::USING;
			}
			else {
				status = Client::ONLINE;
			}
			std::cout.flush();
			break;
		// 消息发送业务
		case Client::USING:
			status = Client::ONLINE;
			{
				// 客户端发送线程
				ScopedThread st1(std::thread(&Client::sendThread, std::ref(cln), sock));
				// 客户端接收线程
				ScopedThread st2(std::thread(&Client::recvThread, std::ref(cln), sock));
			}
			std::cout << LOGOUT_SUCCESS << std::endl;
			break;
		}
	}
	std::cout << THANK_MESSAGE << std::endl;
}

// 发送线程，只负责向服务器发送消息
void Client::sendThread(Client & cln, SOCKET s) {
	char buffer[1024];
	while (true) {
		memset(buffer, 0, 1024);
		while (true) {
			std::cout << "@";
			std::cin.getline(buffer, 1024);
			cln.sendContent.assign(buffer);
			// 将用户输入转换成服务器能识别的形式
			if (counterInterprete(cln.userName, cln.sendContent)) {
				if (cln.sendContent[0] == '3') {
					cln.bridge = true;
				}
				break;
			}
			else
				std::cout << "发送信息格式有误！" << std::endl;
		}
		send(s, cln.sendContent.c_str(), cln.sendContent.size(), 0);
		if (cln.bridge)
			break;
	}
	
}
// 接收线程，只负责接收服务器发来的消息
void Client::recvThread(Client & cln, SOCKET s) {
	int totalLen = 0, msgLen;
	char buffer[1024];
	MESSAGE msg;
	while (true) {
		memset(buffer, 0, 1024);
		recv(s, buffer, 1024, 0);
		cln.recvContent.assign(buffer);
		totalLen = (cln.recvContent).size();
		// 解析服务器发来的消息
		while (totalLen > 0) {
			msg = interprete(cln.recvContent, 0);
			msgLen = messageLength(msg);
			(cln.sendContent).erase(0, msgLen);
			totalLen -= msgLen;
			std::cout << "From:" << msg.subject << "  " << msg.content << std::endl;
		}
		if (cln.bridge)
			break;
	}
}
// 显示帮助页面
void Client::help() {
	std::cout << "EasyChat 提供多人在线聊天功能。" << std::endl;
	std::cout << "常见问题：" << std::endl;
	std::cout << "如何发送信息：用户名 + 空格 + 您要发送的内容。" << std::endl;
	std::cout << "如何退出登录：发送SERVER + 空格 + QUIT即可退出登录。" << std::endl;
	std::cout << "为什么注册失败：可能出现与他人重名的情况，密码不允许为空。" << std::endl;
	std::cout << "为什么信息发送出去后，对方没有回应：对方也许目前不在线，登录后可收到您发送的消息。" << std::endl;
	system("pause");
}
