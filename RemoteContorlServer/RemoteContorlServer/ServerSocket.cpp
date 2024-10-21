#include "pch.h"
#include"ServerSocket.h"

SOCKET*  CServerSocket::m_sockClientPtr = nullptr;

CServerSocket::CServerSocket()
{
	TRACE("����ģ������\r\n");
	CServerSocket::m_sockClientPtr = &this->m_sockClient;
	BOOL ret =  this->InitSocketEnv();
	if (!ret)
	{
		AfxMessageBox(_T("��ʼ�����绷��ʧ��!\r\n"));
		exit(-1);
	}
}

CServerSocket::~CServerSocket()
{
	WSACleanup();
}

CServerSocket::CServerSocket(const CServerSocket& contorller)
{
	if (&contorller == this)
	{
		return;
	}
}

CServerSocket& CServerSocket::operator= (const CServerSocket& contorller)
{
	if (&contorller == this)
	{
		return *this;
	}
	return *this;
}

void CServerSocket::releaseInstance()
{
	if (CServerSocket::m_instance != nullptr)
	{
		delete CServerSocket::m_instance;
		CServerSocket::m_instance = nullptr;
	}
}

CServerSocket* CServerSocket::getInstance()
{
	if (CServerSocket::m_instance == nullptr)
	{
		CServerSocket::m_instance = new CServerSocket();
	}
	return CServerSocket::m_instance;
}

BOOL CServerSocket::InitSocketEnv()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		TRACE("WSAStartup failed with error: %d\n", err);
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {		
		TRACE("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}

BOOL CServerSocket::InitSocketMouseEvent()
{
	if (this->m_socketMouseEvent != INVALID_SOCKET)
	{
		this->CloseSocketMouseEvent();
	}
	this->m_socketMouseEvent = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_socketMouseEvent == SOCKET_ERROR)
	{
		TRACE("socket Error:%d\r\n", WSAGetLastError());
		return FALSE;
	}

	this->m_sockAddrServerMouseEvent.sin_port = htons(8888);
	this->m_sockAddrServerMouseEvent.sin_family = AF_INET;
	this->m_sockAddrServerMouseEvent.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(this->m_socketMouseEvent,(SOCKADDR*)&this->m_sockAddrServerMouseEvent,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		TRACE("bind Error:%d\r\n",WSAGetLastError());
		return FALSE;
	}

	if (listen(this->m_socketMouseEvent,5) == SOCKET_ERROR)
	{
		TRACE("listen Error:%d\r\n",WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}
 
void CServerSocket::CloseSocketMouseEvent()
{
	if (this->m_socketMouseEvent != INVALID_SOCKET || this->m_socketMouseEventClient != INVALID_SOCKET)
	{
		closesocket(this->m_socketMouseEvent);
		closesocket(this->m_socketMouseEventClient);
		this->m_socketMouseEventClient = INVALID_SOCKET;
		this->m_socketMouseEvent = INVALID_SOCKET;
	}
}

BOOL CServerSocket::InitSocket()
{
	if (this->m_sockServer != INVALID_SOCKET)
	{
		this->CloseSocket();
	}
	this->m_sockServer = socket(AF_INET,SOCK_STREAM,0);
	if (this->m_sockServer == INVALID_SOCKET)
	{
		TRACE("socket Error��%s[%d]%s errno:%d \r\n",__FILE__,__LINE__,__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
	this->m_sockAddrSer.sin_family = AF_INET;
	this->m_sockAddrSer.sin_port = htons(9527);
	this->m_sockAddrSer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(this->m_sockServer,(SOCKADDR*)&this->m_sockAddrSer,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		TRACE("bind Error:%s[%d]%s errno:%d\r\n",__FILE__,__LINE__,__FUNCTION__,WSAGetLastError());
		return FALSE;
	}

	if (listen(this->m_sockServer,5) == SOCKET_ERROR)
	{
		TRACE("listen Error:%s[%d]%s errno:%d\r\n", __FILE__, __LINE__, __FUNCTION__, WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL CServerSocket::AcceptClientMouseEvent()
{
	int len = sizeof(SOCKADDR);
	if ((this->m_socketMouseEventClient =  accept(this->m_socketMouseEvent,(SOCKADDR*)&this->m_sockAddrClientMouseEvent,&len)) == SOCKET_ERROR)
	{
		TRACE("accept Error:%d\r\n",WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL CServerSocket::AcceptClient()
{
	int len = sizeof(SOCKADDR);
	if ((this->m_sockClient =  accept(this->m_sockServer,(SOCKADDR*)&this->m_sockAddrCli,&len)) == INVALID_SOCKET)
	{
		TRACE("accept Error:%s[%d]%s errno:%d\r\n",__FILE__,__LINE__,__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
	int size = 1024 * 1024;
	setsockopt(this->m_sockClient,SOL_SOCKET,SO_SNDBUF,(const char*)&size,sizeof(size));
	setsockopt(this->m_sockClient,SOL_SOCKET,SO_RCVBUF,(const char*)&size,sizeof(size));
	return TRUE;
}

void CServerSocket::CloseSocket()
{
	if (this->m_sockServer != INVALID_SOCKET || this->m_sockClient != INVALID_SOCKET)
	{
		closesocket(this->m_sockServer);
		closesocket(this->m_sockClient);
		this->m_sockServer = INVALID_SOCKET;
		this->m_sockClient = INVALID_SOCKET;
	}
}

int CServerSocket::DealCommand()
{
	//�������Կͻ��˷��͹����ĵ���������������
	this->RecvAllDataOfOnePacket();
	if (this->m_packet.getCmd() == 0) //�����������
	{
		//�˳�����,���ó�����������е�����
		this->CloseSocket();
		exit(0);
	}
	//���ݰ�������������Command����ͬ�Ĳ�����������Ҫ��ɵĶ����ӵ��̳߳ؽ������
	CCommand* pObj = (CCommand*)this->m_object;
	pObj->ExecCommand(this->m_packet,this->m_sendList);
	//�������Ľ�����뷢�Ͷ����У����ҽ����Ͷ����е�����ȫ������
}

int CServerSocket::DealCommandMouseEvent()
{
	//�������Կͻ��˷��͹����ĵ���������������
	this->RecvAllDataOfOnePacketMouseEvent();
	if (this->m_packetMouseEvent.getCmd() == 0) //�����������
	{
		//�˳�����,���ó�����������е�����
		this->CloseSocket();
		exit(0);
	}
	//���ݰ�������������Command����ͬ�Ĳ�����������Ҫ��ɵĶ����ӵ��̳߳ؽ������
	CCommand* pObj = (CCommand*)this->m_object;
	pObj->ExecCommandMouseEvent(this->m_packetMouseEvent,this->m_sendListMouseEvent);
	//�������Ľ�����뷢�Ͷ����У����ҽ����Ͷ����е�����ȫ������
}

unsigned CServerSocket::threadEntryRunServer(LPVOID arg)
{
	CServerSocket* thiz = (CServerSocket*)arg;
	thiz->RunMouseEvent();
	_endthreadex(0);
	return 0;
}

void CServerSocket::RunMouseEvent()
{
	while (true)
	{
		BOOL ret = this->InitSocketMouseEvent();
		if (!ret)
		{
			AfxMessageBox(_T("��ʼ���׽���ʧ��!\r\n"));
			return;
		}
		ret = this->AcceptClientMouseEvent();
		if (!ret)
		{
			AfxMessageBox(_T("�ͻ�������ʧ��!\r\n"));
			return;
		}
		this->DealCommandMouseEvent();
		WaitForSingleObject(((CCommand*)this->m_object)->m_signalMouseEvent, INFINITE);
		/*while (this->m_sendListMouseEvent.size() > 0)
		{
			CPacket pack = this->m_sendListMouseEvent.front();
			this->SendAllDataOfOnePacketMouseEvent(pack);
			this->m_sendListMouseEvent.pop_front();
		}*/
		ResetEvent(((CCommand*)this->m_object)->m_signalMouseEvent);
		this->CloseSocketMouseEvent();
	}
}

void CServerSocket::RecvAllDataOfOnePacket()  //ȷ��һ�����ݰ����ܴ��������
{
	char* recvBuffer = new char[1024050];
	memset(recvBuffer, 0, sizeof(recvBuffer));

	//ÿ�ν���102400���ֽ�
	size_t alReadlyToRecv = 0;
	size_t stepSize = 1024050;
	//    char* pData = this->m_recvBuffer.data();
	char* pData = recvBuffer;
	//���յ������ݰ�����������
	int ret = recv(this->m_sockClient, pData + alReadlyToRecv, stepSize, 0);
	CTestTool::Dump((const BYTE*)recvBuffer, ret);
	std::string data(recvBuffer, ret);
	CTestTool::Dump((const BYTE*)data.c_str(), ret);
	//���н��
	size_t nSize = ret;
	this->m_packet = CPacket((const BYTE*)recvBuffer, nSize);
	TRACE("��������ĵ��ֽ���:%d\r\n", nSize);
	delete[]recvBuffer;
}

//TODO:д���⣬������
void CServerSocket::RecvAllDataOfOnePacketMouseEvent()
{
	char recvBuffer[1024] = {0};
	memset(recvBuffer,0,sizeof(recvBuffer));
	size_t alRealdyToRecv = 0; //�Ѿ����յ��ֽ���
	size_t chunkSize = 1024; //ÿ�ν��յĴ�С
	char* pData = recvBuffer;

	size_t ret = recv(this->m_socketMouseEventClient, pData + alRealdyToRecv, chunkSize, 0);
	CTestTool::Dump((const BYTE*)pData,ret);	
	//���н��
	size_t nSize = ret;
	this->m_packetMouseEvent = CPacket((const BYTE*)recvBuffer,nSize);
	TRACE("��������ĵ��ֽ���:%d\r\n", nSize);
}


//TODO:���ż�������
void CServerSocket::RunServer(LPVOID object)
{
	this->m_object = object;
	//����������괦���߳�
	_beginthreadex(nullptr,0,&CServerSocket::threadEntryRunServer,this,0,nullptr);
	while (true)
	{
	  BOOL ret = this->InitSocket();
	  if (!ret)
	  {
		  AfxMessageBox(_T("��ʼ���׽���ʧ��!\r\n"));
		  return;
	  }
	  ret = this->AcceptClient();
	  if (!ret)
	  {
		  AfxMessageBox(_T("�ͻ�������ʧ��!\r\n"));
		  return;
	  }
	  this->DealCommand();
	  WaitForSingleObject(((CCommand*)this->m_object)->m_signal,INFINITE);  //ִ�е������������
	  while (this->m_sendList.size() > 0)
	  {
		  CPacket pack =  this->m_sendList.front();
		  this->SendAllDataOfOnePacket(pack);
		  this->m_sendList.pop_front();
	  }

	  //���͵���Ƶ֡�������뷢�Ͷ�����
	  if (CCommand::packet != nullptr)
	  {
		  //���н������з���
		  size_t ret =  send(this->m_sockClient,CCommand::packet,CCommand::nSize,0);
		  Sleep(1);
		  delete[] CCommand::packet;
		  CCommand::packet = nullptr;
		  CCommand::nSize = 0;
	  }
	  ResetEvent(((CCommand*)this->m_object)->m_signal);
	  this->CloseSocket();
	}
}

SOCKET& CServerSocket::getSockClient()
{
	return  *CServerSocket::m_sockClientPtr;
}

size_t CServerSocket::SendAllDataOfOnePacket(CPacket& packet)
{
	size_t alReadyToSend = 0;
	size_t notToSend = packet.getDataLenght()+6;
	size_t stepSize = 102400;
	std::string data = "";
	std::vector<char> strData;
	strData.resize(packet.getDataLenght()+6);
	char* pData = strData.data();
	memset(pData,0,packet.getDataLenght()+6);
	packet.toByteData(data);
	memcpy(pData,data.c_str(),data.size());  //�����µ�ԭ���ǳ����������ĳ���
	TRACE("�����뷢�ͻ����������ݳ��ȵĴ�СΪ:%d\r\n", strData.size());
	CTestTool::Dump((const BYTE*)pData,data.size());

	while (alReadyToSend < notToSend)
	{
		size_t ret = send(this->m_sockClient,pData + alReadyToSend,notToSend,0);
		if (ret > 0)
		{
			alReadyToSend += ret;
			notToSend -= ret;
		}
		else
		{
			break;
		}
	}
	return alReadyToSend;
}

size_t CServerSocket::SendAllDataOfOnePacketMouseEvent(CPacket& packet)
{
	size_t alReadyToSend = 0;
	size_t notToSend = packet.getDataLenght() + 6;
	size_t stepSize = 102400;
	std::string data = "";
	std::vector<char> strData;
	strData.resize(packet.getDataLenght() + 6);
	char* pData = strData.data();
	memset(pData, 0, packet.getDataLenght() + 6);
	packet.toByteData(data);
	memcpy(pData, data.c_str(), data.size());  //�����µ�ԭ���ǳ����������ĳ���
	TRACE("�����뷢�ͻ����������ݳ��ȵĴ�СΪ:%d\r\n", strData.size());
	CTestTool::Dump((const BYTE*)pData, data.size());

	while (alReadyToSend < notToSend)
	{
		size_t ret = send(this->m_socketMouseEventClient, pData + alReadyToSend, notToSend, 0);
		if (ret > 0)
		{
			alReadyToSend += ret;
			notToSend -= ret;
		}
		else
		{
			break;
		}
	}
	return alReadyToSend;
}

CServerSocket::CHelper CServerSocket::m_helper;
CServerSocket* CServerSocket::m_instance = nullptr;
