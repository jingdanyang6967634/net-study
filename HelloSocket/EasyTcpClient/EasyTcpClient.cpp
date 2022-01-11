// EasyTcpClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<thread>
#include "EasyTcpClient.hpp"

using namespace std;
#pragma comment(lib,"ws2_32.lib")


bool g_bRun = true;
void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			cout << "退出" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lyd");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Loginout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else
		{
			cout << "不支持的命令" << endl;
		}
	}
}
int main()
{
	EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1",4567);
	//启动ui线程
	std::thread t1(cmdThread,&client);
	t1.detach();

	while (client.isRun())
	{
		client.OnRun();		
	}
	client.Close();
	cout << "exit system" << endl;
	getchar();
	return 0;
}