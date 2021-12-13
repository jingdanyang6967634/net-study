// EasyTcpClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<WinSock2.h>
#include<windows.h>

using namespace std;
#pragma comment(lib,"ws2_32.lib")
struct DataPackage
{
	int age;
	char name[32];
};

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
		//3.输入请求命令
		//scanf("%s",cmdBuf);
		cin >> cmdBuf;
		//4.处理请求命令
		if (0 == strcmp(cmdBuf,"exit"))
		{
			cout << "receive exit cmd" << endl;
			break;
		}
		else if(0 == strcmp(cmdBuf, "login"))
		{
			Login login = {"lyd","lydmm"};
			DataHeader dh = { sizeof(Login),CMD_LOGIN };
			//5.向服务器发送请求命令
			send(_sock, (const char *)&dh,sizeof(DataHeader),0);
			send(_sock, (const char *)&login, sizeof(Login), 0);
			//接收服务器返回数据
			DataHeader retHeader = {};
			LoginResult loginRet = {};
			recv(_sock, (char *)&retHeader,sizeof(DataHeader),0);
			recv(_sock, (char *)&loginRet, sizeof(LoginResult), 0);

			cout << "LoginResult :" << loginRet.result << endl;
		}
		else if (0 == strcmp(cmdBuf, "loginout"))
		{
			Loginout logout = {"lyd"};
			DataHeader dh = { sizeof(Loginout),CMD_LOGINOUT };
			//5.向服务器发送请求命令
			send(_sock,(const char*)&dh, sizeof(DataHeader), 0);
			send(_sock,(const char*)&logout, sizeof(Loginout),0);

			DataHeader retHeader = {};
			LoginoutResult logoutRet = {};
			recv(_sock, (char *)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char *)&logoutRet, sizeof(LoginoutResult), 0);
			cout << "LoginoutResult :" << logoutRet.result << endl;
		}
		else
		{
			cout << "not support cmd" << endl;
		}
	}
	//7.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();//关闭
	cout << "exit system" << endl;
	getchar();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
