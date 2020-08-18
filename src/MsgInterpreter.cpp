#include "stdafx.h"
#include "CommenHeaders.h"
#include "MsgInterpreter.h"

// ����MESSAGE�ṹ���ַ������ܳ���
int messageLength(MESSAGE & msg) {
	return msg.subject.size() + msg.object.size() + msg.content.size() + 4;
}

// ��������������������Ϣ��������һ��MESSAGE�ṹ����
MESSAGE interprete(string & bytes, int flag) {
	MESSAGE msg{ 0 };
	int length = bytes.size(), begin = 1, after = 0;
	char type = bytes[0];
	switch (flag) {
	case 0:
		switch (type) {
		case '1':
			msg.type = 1;
			break;
		case '2':
			msg.type = 2;
			break;
		case '3':
			msg.type = 3;
			break;
		case '4':
			msg.type = 4;
			break;
		}
		after = bytes.find('&', 0);
		msg.object = bytes.substr(begin, after - begin);
		begin = after + 1;
		after = bytes.find('&', begin);
		msg.subject = bytes.substr(begin, after-begin);
		begin = after + 1;
		after = bytes.find('&', begin);
		if (after == std::string::npos) {
			msg.content = bytes.substr(begin);
		}
		else {
			msg.content = bytes.substr(begin, after - begin);
		}
	default:
		break;
	}
	return msg;
}

// ���û�����ת��Ϊ����ϵͳ��Ϣ��ʽ
bool counterInterprete(const std::string & userName, std::string & content) {
	int pos = content.find(' ');
	std::string object;
	if (pos == std::string::npos || pos == 0)
		return false;
	object = content.substr(0, pos);
	content = content.substr(pos + 1);
	if (object == "SERVER") {
		if (content == "QUIT") {
			content = "3" + object + "&" + userName + "&" + content;
		}
		else if(content == "REGIST") {
			content = "4" + object + "&" + userName + "&" + content;
		}
	}
	else {
		content = "2" + object + "&" + userName + "&" + content;
	}
	return true;
}