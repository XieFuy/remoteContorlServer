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
	SOCKET m_socketMouseEvent;
	SOCKADDR_IN m_sockAddrServerMouseEvent;
	SOCKET m_socketMouseEventClient;
	SOCKADDR_IN m_sockAddrClientMouseEvent;
	LPVOID m_object;
	SOCKADDR_IN m_sockAddrSer;
	SOCKADDR_IN m_sockAddrCli;
	SOCKET m_sockClient;
	static SOCKET* m_sockClientPtr;
	SOCKET m_sockServer;
	std::list<CPacket> m_sendListMouseEvent;
	std::list<CPacket> m_sendList; //发送队列
	CPacket m_packet;
	CPacket m_packetMouseEvent;
	static CServerSocket* m_instance;
	std::vector<char> m_buffer; //使用动态数组来进行接收数据
	BOOL InitSocketEnv(); //初始化网络环境
	BOOL InitSocket(); //初始化套接字
	BOOL InitSocketMouseEvent();
	void CloseSocketMouseEvent();
	void CloseSocket();//关闭套接字
	BOOL AcceptClient();//接收客户端连接请求
	BOOL AcceptClientMouseEvent();
	int DealCommandMouseEvent();
	int DealCommand(); //处理客户端发送过来的命令
	void RecvAllDataOfOnePacketMouseEvent();
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
	static unsigned WINAPI threadEntryRunServer(LPVOID arg);//进入第二个鼠标线程的入口线程函数
	void RunMouseEvent();
	size_t SendAllDataOfOnePacket(CPacket& packet); //将数据包发送到客户端
	size_t SendAllDataOfOnePacketMouseEvent(CPacket& packet);
	static SOCKET& getSockClient();
};

