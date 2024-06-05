#pragma once
#include"Packet.h"
#include"Command.h"
#include<vector>
#include<list>
#include"TestTool.h"

//网络模块
class CServerSocket
{
private:
	
	LPVOID m_object;
	SOCKADDR_IN m_sockAddrSer;
	SOCKADDR_IN m_sockAddrCli;
	SOCKET m_sockClient;
	SOCKET m_sockServer;
	std::list<CPacket> m_sendList; //发送队列
	CPacket m_packet;
	static CServerSocket* m_instance;
	std::vector<char> m_buffer; //使用动态数组来进行接收数据
	BOOL InitSocketEnv(); //初始化网络环境
	BOOL InitSocket(); //初始化套接字
	void CloseSocket();//关闭套接字
	BOOL AcceptClient();//接收客户端连接请求
	int DealCommand(); //处理客户端发送过来的命令
	void RecvAllDataOfOnePacket(); //接收单个数据包的所有数据
	CServerSocket();
	~CServerSocket();
	CServerSocket(const CServerSocket& contorller);
	CServerSocket& operator= (const CServerSocket& contorller);
	static void releaseInstance();
	class CHelper 
	{
	public:
		CHelper()
		{
			CServerSocket::getInstance();
		}
		~CHelper() 
		{
			CServerSocket::releaseInstance();
		}
	};
	static CHelper m_helper;
public:
	static CServerSocket* getInstance();
	void RunServer(LPVOID object);//启动服务器  传入一个Command对象
	size_t SendAllDataOfOnePacket(CPacket& packet); //将数据包发送到客户端
};

