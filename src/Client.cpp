#include "stdafx.h"
#include "CommenHeaders.h"
#include "Client.h"
#include "MsgInterpreter.h"
#include "ScopedThread.h"
#include "Constants.h"

// Client�߳�
Client::Client(int p_) :port(p_),bridge(false) {

}

// ����ͻ��˵������������Ӻ͵�¼����Ϣע���ҵ��
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
	// ѭ���������ҵ��
	while (status != Client::QUIT) {
		switch (status) {
		// ��¼ҵ��
		case Client::LOGIN:
			cln.bridge = false;
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			//���������������
			memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
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
			std::cout << "����������û�����";
			std::cin >> userName;
			std::cout << "������������룺";
			std::cin >> passWord;
			std::cin.get();
			(cln.sendContent) = (std::string)"1Server" + SPLITTER + userName + SPLITTER + passWord;
			send(sock, (cln.sendContent).c_str(), (cln.sendContent).size(), 0);
			len = recv(sock, buffer, 1024, 0);
			(cln.recvContent).assign(buffer);
			// �������������صĵ�¼��Ϣ
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
		// ����ѡ�����
		case Client::ONLINE:
			std::cout << "��������������" << std::endl;
			std::cout << "1. ��½" << std::endl;
			std::cout << "2. ע��" << std::endl;
			std::cout << "3. ����" << std::endl;
			std::cout << "4. �˳�" << std::endl;
			std::cin.getline(buffer, 1024);
			option.assign(buffer);
			// �л��ͻ��˵�״̬
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
				std::cout << "����������ѡ����Ӧ�����֣�" << std::endl;
			}
			break;
		// ע��ҵ��
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
			std::cout << "������ע����û�����";
			std::cin >> userName;
			std::cout << "�������������룺";
			std::cin >> passWord;
			std::cin.get();
			// ����ע����Ϣ
			cln.sendContent = (std::string)"4" + HOST + SPLITTER + userName + SPLITTER + passWord;
			send(sock, (cln.sendContent).c_str(), (cln.sendContent).size(), 0);
			recv(sock, buffer, 1024, 0);
			(cln.recvContent).assign(buffer);
			msg = interprete(cln.recvContent, 0);
			std::cout << "From:" << msg.subject << " " << msg.content << std::endl;
			// ���ݷ��������������Ϣ�л�״̬
			if (msg.content == REGIST_SUCCESS) {
				cln.userName = userName;
				status = Client::USING;
			}
			else {
				status = Client::ONLINE;
			}
			std::cout.flush();
			break;
		// ��Ϣ����ҵ��
		case Client::USING:
			status = Client::ONLINE;
			{
				// �ͻ��˷����߳�
				ScopedThread st1(std::thread(&Client::sendThread, std::ref(cln), sock));
				// �ͻ��˽����߳�
				ScopedThread st2(std::thread(&Client::recvThread, std::ref(cln), sock));
			}
			std::cout << LOGOUT_SUCCESS << std::endl;
			break;
		}
	}
	std::cout << THANK_MESSAGE << std::endl;
}

// �����̣߳�ֻ�����������������Ϣ
void Client::sendThread(Client & cln, SOCKET s) {
	char buffer[1024];
	while (true) {
		memset(buffer, 0, 1024);
		while (true) {
			std::cout << "@";
			std::cin.getline(buffer, 1024);
			cln.sendContent.assign(buffer);
			// ���û�����ת���ɷ�������ʶ�����ʽ
			if (counterInterprete(cln.userName, cln.sendContent)) {
				if (cln.sendContent[0] == '3') {
					cln.bridge = true;
				}
				break;
			}
			else
				std::cout << "������Ϣ��ʽ����" << std::endl;
		}
		send(s, cln.sendContent.c_str(), cln.sendContent.size(), 0);
		if (cln.bridge)
			break;
	}
	
}
// �����̣߳�ֻ������շ�������������Ϣ
void Client::recvThread(Client & cln, SOCKET s) {
	int totalLen = 0, msgLen;
	char buffer[1024];
	MESSAGE msg;
	while (true) {
		memset(buffer, 0, 1024);
		recv(s, buffer, 1024, 0);
		cln.recvContent.assign(buffer);
		totalLen = (cln.recvContent).size();
		// ������������������Ϣ
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
// ��ʾ����ҳ��
void Client::help() {
	std::cout << "EasyChat �ṩ�����������칦�ܡ�" << std::endl;
	std::cout << "�������⣺" << std::endl;
	std::cout << "��η�����Ϣ���û��� + �ո� + ��Ҫ���͵����ݡ�" << std::endl;
	std::cout << "����˳���¼������SERVER + �ո� + QUIT�����˳���¼��" << std::endl;
	std::cout << "Ϊʲôע��ʧ�ܣ����ܳ�����������������������벻����Ϊ�ա�" << std::endl;
	std::cout << "Ϊʲô��Ϣ���ͳ�ȥ�󣬶Է�û�л�Ӧ���Է�Ҳ��Ŀǰ�����ߣ���¼����յ������͵���Ϣ��" << std::endl;
	system("pause");
}
