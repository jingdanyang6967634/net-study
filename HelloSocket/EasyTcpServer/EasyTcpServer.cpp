// EasyTcpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<windows.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength; 
	short cmd;
};
struct Login
{
	char userName[32];
	char PassWord[32];
};
struct LoginResult
{
	int result;
};
struct Loginout
{
	char userName[32];
};
struct LoginoutResult
{
	int result;
};

using namespace std;
int main()
{
	WORD ver = MAKEWORD(2, 2);//调用API2代创建2.x版本
	WSADATA dat;
	WSAStartup(ver, &dat);//启动
	

	//1.建立一个socket
	SOCKET  _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//访问本机任何网络地址都可以，具体也可以改为本机的某个特定地址
	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in)))
	{
		cout << "bind fail" << endl;
	}
	else
	{
		cout << "bind success" << endl;
	}
	//3.listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		cout << "listen fail" << endl;
	}
	else
	{
		cout << "listen success" << endl;
	}
	//4.accept 等待接受客户端连接
	sockaddr_in clientAddr;
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;

	

	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		cout << "accept invalid client socket" << endl;
	}
	cout << "new client add, socket = " << _cSock << " ip="  << inet_ntoa(clientAddr.sin_addr) << endl;
	
	char _recvBuf[128] = {};
	while (true)
	{
		DataHeader header = {};
		//5.接收客户端数据
		int nLen = recv(_cSock, (char *)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			cout << "client already exit";
			break;
		}
		cout << "recevie cmd: " << header.cmd << "data length:" << header.dataLength << endl;
		//6.处理请求
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char *)&login, sizeof(Login), 0);
			//忽略判断用户密码是否正确的过程
			LoginResult ret = { 1 };
			send(_cSock, (char *)&header, sizeof(DataHeader), 0);
			send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			Loginout loginout = {};
			recv(_cSock, (char *)&loginout, sizeof(Loginout), 0);
			//忽略判断用户密码是否正确的过程
			LoginoutResult ret = { 1 };
			send(_cSock, (char *)&header, sizeof(DataHeader), 0);
			send(_cSock, (char *)&ret, sizeof(LoginoutResult), 0);
		}
		break;
		default:
		{
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_cSock, (char *)&header, sizeof(DataHeader), 0);
		}
		break;
		}
	}
	
   // 8.关闭套接字closesocket
	closesocket(_cSock);


    //清楚windows socket环境
	WSACleanup();//关闭
	cout << "exit system" << endl;
	getchar();
	return 0;
}




