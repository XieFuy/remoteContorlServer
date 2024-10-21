#pragma once
#include"Packet.h"
#include"Command.h"
#include<vector>
#include<list>
#include"TestTool.h"

//����ģ��
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
	std::list<CPacket> m_sendList; //���Ͷ���
	CPacket m_packet;
	CPacket m_packetMouseEvent;
	static CServerSocket* m_instance;
	std::vector<char> m_buffer; //ʹ�ö�̬���������н�������
	BOOL InitSocketEnv(); //��ʼ�����绷��
	BOOL InitSocket(); //��ʼ���׽���
	BOOL InitSocketMouseEvent();
	void CloseSocketMouseEvent();
	void CloseSocket();//�ر��׽���
	BOOL AcceptClient();//���տͻ�����������
	BOOL AcceptClientMouseEvent();
	int DealCommandMouseEvent();
	int DealCommand(); //����ͻ��˷��͹���������
	void RecvAllDataOfOnePacketMouseEvent();
	void RecvAllDataOfOnePacket(); //���յ������ݰ�����������
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
	void RunServer(LPVOID object);//����������  ����һ��Command����
	static unsigned WINAPI threadEntryRunServer(LPVOID arg);//����ڶ�������̵߳�����̺߳���
	void RunMouseEvent();
	size_t SendAllDataOfOnePacket(CPacket& packet); //�����ݰ����͵��ͻ���
	size_t SendAllDataOfOnePacketMouseEvent(CPacket& packet);
	static SOCKET& getSockClient();
};

