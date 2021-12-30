// EasyTcpClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<WinSock2.h>
#include<windows.h>

using namespace std;
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

int processor(SOCKET _cSock)
{
	char szRecv[4096] = {};
	//5.接收客户端数据	
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader *header = (DataHeader *)szRecv;
	if (nLen <= 0)
	{
		cout << "与服务器断开连接，任务结束" << _cSock << endl;
		return -1;
	}
	//6.处理请求
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult *login = (LoginResult *)szRecv;

			cout << "receive server data: CMD_LOGIN_RESULT,"  << "data length:" << login->dataLength << endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginoutResult *logout = (LoginoutResult *)szRecv;

			cout << "receive server data: LoginoutResult," << "data length:" << logout->dataLength << endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin *userJoin = (NewUserJoin *)szRecv;

			cout << "receive server data: NewUserJoin," << "data length:" << userJoin->dataLength << endl;
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
	SOCKET  _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		cout << "invalid socket" << endl;
	}
	//2.连接服务器connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock,(sockaddr*)(&_sin),sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		cout << "connect error" << endl;
	}
	char cmdBuf[128] = {};
	while (true)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval t = {1,0};
		int ret = select(_sock, &fdReads,0,0,&t);
		if (ret < 0)
		{
			cout << "select任务结束1" << endl;
			break;
		}
		if(FD_ISSET(_sock,&fdReads))
		{
			FD_CLR(_sock,&fdReads);
			if (-1 == processor(_sock))
			{
				cout << "select任务结束2" << endl;
				break;
			}
		}
		cout << "空闲时间处理其他业务。。。" << endl;
		Login login;
		strcpy(login.userName,"lyd");
		strcpy(login.PassWord,"lyd");
		send(_sock,(const char*)&login,sizeof(Login),0);
		Sleep(1000);
	}
	//7.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();//关闭
	cout << "exit system" << endl;
	getchar();
	return 0;
}