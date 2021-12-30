// EasyTcpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<windows.h>
#include<iostream>

#include<vector>
#pragma comment(lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength; 
	short cmd;
};
struct Login : public DataHeader
{
	//DataHeader header;
	Login() 
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct Loginout : public DataHeader
{
	Loginout()
	{
		dataLength = sizeof(Loginout);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginoutResult : public DataHeader
{
	LoginoutResult()
	{
		dataLength = sizeof(LoginoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
using namespace std;

vector<SOCKET> g_clients;
int processor(SOCKET _cSock)
{
	char szRecv[4096] = {};
	//5.接收客户端数据	
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader *header = (DataHeader *)szRecv;
	if (nLen <= 0)
	{
		cout << "client  _cSock:"<< _cSock << "already exit";
		return -1;
	}
	//6.处理请求
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{

		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login *login = (Login *)szRecv;

		cout << "receive client socket:" << _cSock << "recevie cmd: " << login->cmd << "data length:" << login->dataLength << "userName:" << login->userName << endl;
		//忽略判断用户密码是否正确的过程
		LoginResult ret;
		//send(_cSock, (char *)&header, sizeof(DataHeader), 0);
		send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{

		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Loginout *loginout = (Loginout *)szRecv;
		cout << "receive client socket:" << _cSock << "recevie cmd: " << loginout->cmd << "data length:" << loginout->dataLength << "userName:" << loginout->userName << endl;
		//忽略判断用户密码是否正确的过程
		LoginoutResult ret;
		//send(_cSock, (char *)&header, sizeof(DataHeader), 0);
		send(_cSock, (char *)&ret, sizeof(LoginoutResult), 0);
	}
	break;
	default:
	{
		DataHeader header = { 0,CMD_ERROR };
		//header->cmd = CMD_ERROR;
		//header->dataLength = 0;
		send(_cSock, (char *)&header, sizeof(DataHeader), 0);
	}
	break;
	}
}
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
	while (true)
	{
		//伯克利 socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size() -1 ; n >=0 ; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}
		timeval t = {1,0};
		int ret = select(_sock+1,&fdRead,&fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			cout << "select任务结束" << endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock,&fdRead);
			//4.accept 等待接受客户端连接
			sockaddr_in clientAddr;
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				cout << "accept invalid client socket" << endl;
			}
			else
			{
				//新用户加入群发消息
				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					NewUserJoin userJoin;
					send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(_cSock);
				cout << "new client add, socket = " << (int)_cSock << " ip=" << inet_ntoa(clientAddr.sin_addr) << endl;
			}	
		}

		for (size_t n = 0 ; n < fdRead.fd_count; n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if(iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}

		cout << "空闲时间处理其他业务。。。" << endl;
	}
	
	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}


   // 8.关闭套接字closesocket
	closesocket(_sock);
    //清楚windows socket环境
	WSACleanup();//关闭
	cout << "exit system" << endl;
	getchar();
	return 0;
}




