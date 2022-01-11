#pragma once
#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include<WinSock2.h>
	#include<windows.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(-0)
	#define SOCKET_ERROR(-1)
#endif // _WIN32
#include <iostream>
#include "MessageHeader.hpp"
using namespace std;
class EasyTcpClient
{
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpClient()
	{
		Close();
	}

	int InitSocket() 
	{
		WORD ver = MAKEWORD(2, 2);//����API2������2.x�汾
		WSADATA dat;
		WSAStartup(ver, &dat);//����

		if (INVALID_SOCKET == _sock) 
		{
			Close();
		}
	    _sock = socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == _sock)
		{
			cout << "invalid socket" << endl;
		}
		return 0;
	}

	int Connect(const char* ip,unsigned short port) 
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		int ret = connect(_sock, (sockaddr*)(&_sin), sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			cout << "connect error" << endl;
		}
		return ret;
	}

	void Close() 
	{
		if (_sock != INVALID_SOCKET)
		{
			closesocket(_sock);
			WSACleanup();//�ر�
			_sock = INVALID_SOCKET;
		}
	}

	//��������
	//��������
	//����������Ϣ
	bool OnRun() 
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1,0 };
			int ret = select(_sock, &fdReads, 0, 0, &t);
			if (ret < 0)
			{
				cout << "select�������1" << _sock << endl;
				return false;
			}
			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);
				if (-1 == RecvData(_sock))
				{
					cout << "select�������2" << _sock << endl;
					return false;
				}
			}
			return true;
		}
		return false;
	}
	bool isRun() 
	{
		return _sock != INVALID_SOCKET;
	}
	//��������  ����ճ�� ��ְ�
	int RecvData(SOCKET _cSock)
	{
		char szRecv[4096] = {};
		//5.���տͻ�������	
		int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader *header = (DataHeader *)szRecv;
		if (nLen <= 0)
		{
			cout << "��������Ͽ����ӣ��������" << _cSock << endl;
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		OnNetMsg( header);
		return 0;
	}

	void OnNetMsg( DataHeader *header)
	{
		//6.��������
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult *login = (LoginResult *)header;
			cout << "receive server data: CMD_LOGIN_RESULT," << "data length:" << login->dataLength << endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LoginoutResult *logout = (LoginoutResult *)header;
			cout << "receive server data: LoginoutResult," << "data length:" << logout->dataLength << endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin *userJoin = (NewUserJoin *)header;
			cout << "receive server data: NewUserJoin," << "data length:" << userJoin->dataLength << endl;
		}
		break;
		}
	}

	//��������
	int SendData(DataHeader* header) 
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
};


#endif // !_EasyTcpClient_hpp_
