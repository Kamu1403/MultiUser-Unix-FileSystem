#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <list>
#include <string>
#include <sstream>
#include "Session.h"

//Windows网络库
#include <WinSock2.h>
//引入socket动态库，win下可以，但不能跨平台
#pragma comment (lib,"ws2_32.lib")

using namespace std;

const stringstream& mainloop(const string& input,Session* p_Session);

int main()
{
	system("chcp 65001");//防止乱码
	WSADATA wsaData;
	//在window下面需要额外的加载socket库
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "WSAStartup startup failed " << GetLastError() << endl;
		return 0;
	}


	SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == serverSock)
	{
		cout << "launch socket failed " << GetLastError() << endl;
		return 0;
	}


	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(7890);
	if (SOCKET_ERROR == ::bind(serverSock, (sockaddr*)&ServerAddr, sizeof(SOCKADDR_IN)))
	{
		cout << "bind failed" << GetLastError() << endl;
		return 0;
	}


	if (INVALID_SOCKET == listen(serverSock, 5))
	{
		cout << "listen failed " << GetLastError() << endl;
		return 0;
	}


	std::list<SOCKET> listSock;

	while (true)
	{
		cout << "waiting conntection " << endl;
		SOCKADDR_IN clientAddr;
		int clientAddrLen = sizeof(SOCKADDR_IN);
		SOCKET clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientAddrLen);

		cout << "connect succeed: SOCKET: " << clientSock << " ip:" << inet_ntoa(clientAddr.sin_addr) << " port:" << ntohs(clientAddr.sin_port) << endl;

		if (INVALID_SOCKET == clientSock)
			break;
		else
			listSock.push_back(clientSock);

		//多线程 面向多个客户端
		//TIPS：如果将accept模块部分载入多线程，意味着将开启无限多个线程等待客户端的进入，那是没有意义的。
		std::thread t([&listSock]()
			{
				SOCKET clientSock = listSock.back();
				//welcome
				std::thread::id thread_id = this_thread::get_id();
				cout << "reference thread id: " << thread_id << endl;
				stringstream tmp;
				tmp << "your reference thread id: " << thread_id << endl;
				Session* p_session = new Session;
				int nSend = send(clientSock, tmp.str().c_str(), tmp.str().length(), 0);
				const stringstream& ss = mainloop("", p_session);
				nSend = send(clientSock, ss.str().c_str(), ss.str().length(), 0);
				//向连接的客户端收发数据
				while (true)
				{
					auto begin = listSock.begin();
					auto end = listSock.end();
					//收发数据
					char buffer[1024]{ 0 };
					int nRecv = recv(clientSock, buffer, 1024, 0);
					if (nRecv <= 0)
						break;
					buffer[nRecv - 1] = '\0';
					cout << "from socket: " << clientSock << " received  " << nRecv << " length of data: " << buffer << endl;
					const stringstream& ss = mainloop(buffer, p_session);
					//for (; begin != end; begin++)
					//	int nSend = send(*begin, "acknowledged", strlen("acknowledged"), 0);
					int nSend = send(clientSock, ss.str().c_str(), ss.str().length(), 0);
				}
				closesocket(clientSock);
				listSock.remove(clientSock);//移除可参数匹配的元素
				cout << "closing client" << endl;
			});
		t.detach();
	}
	closesocket(serverSock);
	//卸载Windows动态网络库
	WSACleanup();
	system("pause");
	return 0;
}

