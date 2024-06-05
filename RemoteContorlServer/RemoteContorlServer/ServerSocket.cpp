#include "pch.h"
#include"ServerSocket.h"

CServerSocket::CServerSocket()
{
	TRACE("网络模块启动\r\n");
	BOOL ret =  this->InitSocketEnv();
	if (!ret)
	{
		AfxMessageBox(_T("初始化网络环境失败!\r\n"));
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

BOOL CServerSocket::InitSocket()
{
	if (this->m_sockServer != INVALID_SOCKET)
	{
		this->CloseSocket();
	}
	this->m_sockServer = socket(AF_INET,SOCK_STREAM,0);
	if (this->m_sockServer == INVALID_SOCKET)
	{
		TRACE("socket Error：%s[%d]%s errno:%d \r\n",__FILE__,__LINE__,__FUNCTION__,WSAGetLastError());
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

BOOL CServerSocket::AcceptClient()
{
	int len = sizeof(SOCKADDR);
	if ((this->m_sockClient =  accept(this->m_sockServer,(SOCKADDR*)&this->m_sockAddrCli,&len)) == INVALID_SOCKET)
	{
		TRACE("accept Error:%s[%d]%s errno:%d\r\n",__FILE__,__LINE__,__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
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
	//接收来自客户端发送过来的单个包的所有数据
	this->RecvAllDataOfOnePacket();
	if (this->m_packet.getCmd() == 0) //程序结束机制
	{
		//退出程序,做好程序结束的所有的析构
		this->CloseSocket();
		exit(0);
	}
	//根据包的命令来进行Command对象不同的操作，并将需要完成的动作扔到线程池进行完成
	CCommand* pObj = (CCommand*)this->m_object;
	pObj->ExecCommand(this->m_packet,this->m_sendList);
	//将处理后的结果存入发送队列中，并且将发送队列中的内容全部发送
}

void CServerSocket::RecvAllDataOfOnePacket()
{
	this->m_buffer.clear();
	this->m_buffer.resize(1024);
	size_t alRealdyToRecv = 0; //已经接收的字节数
	size_t chunkSize = 1024; //每次接收的大小
	char* pData = this->m_buffer.data();

	size_t ret = recv(this->m_sockClient, pData + alRealdyToRecv, chunkSize, 0);
	CTestTool::Dump((const BYTE*)this->m_buffer.data(), ret);
	std::string data;
	for (std::vector<char>::iterator pos = this->m_buffer.begin();pos != this->m_buffer.end();pos++)
	{
		data.push_back(*pos);
	}
	CTestTool::Dump((const BYTE*)data.c_str(),ret);
	//进行解包
	size_t nSize = ret;
	memset(&this->m_packet,0,sizeof(this->m_packet));
	memcpy(&this->m_packet,data.c_str(),ret);
	//this->m_packet = CPacket((const BYTE*)data.c_str(),nSize);
	TRACE("解包所消耗的字节数:%d\r\n",nSize);
}

//TODO:接着继续往下
void CServerSocket::RunServer(LPVOID object)
{
	this->m_object = object;
	while (true)
	{
	  BOOL ret = this->InitSocket();
	  if (!ret)
	  {
		  AfxMessageBox(_T("初始化套接字失败!\r\n"));
		  return;
	  }
	  ret = this->AcceptClient();
	  if (!ret)
	  {
		  AfxMessageBox(_T("客户端连接失败!\r\n"));
		  return;
	  }
	  this->DealCommand();
	  WaitForSingleObject(((CCommand*)this->m_object)->m_signal,INFINITE);
	  while (this->m_sendList.size() > 0)
	  {
		  CPacket pack =  this->m_sendList.front();
		  this->SendAllDataOfOnePacket(pack);
		  this->m_sendList.pop_front();
	  }
	  ResetEvent(((CCommand*)this->m_object)->m_signal);
	  this->CloseSocket();
	}
}

size_t CServerSocket::SendAllDataOfOnePacket(CPacket& packet)
{
	size_t alReadyToSend = 0;
	size_t notToSend = packet.getDataLenght()+6;
	std::vector<char> strData;
	strData.resize(packet.getDataLenght()+6);
	char* pData = strData.data();
	memset(pData,0,packet.getDataLenght()+6);
	TRACE("sizeof(packet)的大小为:%d\r\n",sizeof(packet));
	memcpy(pData,&packet,packet.getDataLenght()+6);


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

CServerSocket::CHelper CServerSocket::m_helper;
CServerSocket* CServerSocket::m_instance = nullptr;
