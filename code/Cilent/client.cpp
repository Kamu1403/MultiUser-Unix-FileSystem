#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //引入socket动态库，win下可以，但不能跨平台


#include <thread>

using std::cout;
using std::endl;
using std::cin;


int main()
{
	system("chcp 65001");
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (SOCKET_ERROR == nRet)
	{
		cout << "WASStartUp startup failed" << GetLastError() << endl;
		return 0;
	}


	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clientSock)
	{
		cout << "launch socket failed " << GetLastError() << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(7890);

	//connect内部自动给客户端绑定了一个随机端口
	if (INVALID_SOCKET == connect(clientSock, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)))
	{
		cout << "conection failed " << GetLastError() << endl;
		return 0;
	}


	std::thread t([&clientSock]() {
		while (true)
		{
			char buff[1024]{ 0 };
			int nRecvLen = recv(clientSock, buff, 1024, 0);
			if (nRecvLen <= 0)
				break;
			//cout << "received " << nRecvLen << " length of data:" << buff << endl;
			cout << buff;
		}
		});
	t.detach();


	while (true)
	{
		char buffer[1024]{ 0 };
		cin.getline(buffer,1024);
		buffer[strlen(buffer)] = '\n';
		if (buffer[0] == 'q')
			break;
		int nSendLen = send(clientSock, buffer, strlen(buffer), 0);
	}


	closesocket(clientSock);
	WSACleanup();//卸载动态网络库
	system("pause");
	return 0;
}

