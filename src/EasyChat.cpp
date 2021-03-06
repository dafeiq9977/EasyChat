// EasyChat.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CommenHeaders.h"
#include "FileUtils.h"
#include "Server.h"
#include "Client.h"
#include "MsgInterpreter.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main(int argc, char *argv[])
{
	WORD w_req = MAKEWORD(2, 2);
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (argc != 2) {
		std::cout << "传递参数数量错误！" << std::endl;
	}
	// 服务器端代码
	else if (strcmp(argv[1], "Server") == 0) {
		Server t(5010);
		t.start();
	}
	// 客户端代码
	else if(strcmp(argv[1], "Client") == 0){
		Client cln(5010);
		Client::start(cln);
	}
	else {
		cout << "输入错误！第2个参数为Server或Client" << std::endl;
	}
	WSACleanup();
	system("pause");
	return 0;
}

