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
	
	LPVOID m_object;
	SOCKADDR_IN m_sockAddrSer;
	SOCKADDR_IN m_sockAddrCli;
	SOCKET m_sockClient;
	SOCKET m_sockServer;
	std::list<CPacket> m_sendList; //���Ͷ���
	CPacket m_packet;
	static CServerSocket* m_instance;
	std::vector<char> m_buffer; //ʹ�ö�̬���������н�������
	BOOL InitSocketEnv(); //��ʼ�����绷��
	BOOL InitSocket(); //��ʼ���׽���
	void CloseSocket();//�ر��׽���
	BOOL AcceptClient();//���տͻ�����������
	int DealCommand(); //����ͻ��˷��͹���������
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
	size_t SendAllDataOfOnePacket(CPacket& packet); //�����ݰ����͵��ͻ���
};

