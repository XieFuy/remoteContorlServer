#include "pch.h"
#include "Command.h"


int CCommand::testConnect(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("测试：客户端连接成功!\r\n");
	sendLst.push_back(CPacket(1981,nullptr,0));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

CCommand::CCommand()
{
	this->m_signal = CreateEvent(nullptr,FALSE,FALSE,nullptr);
	//加载功能函数到map中
	struct 
	{
		WORD cmd;
		LPFUNC func;
	}data[]{
	  {1,&CCommand::RunFile},
	  {2,&CCommand::DelteRemoteFile},
	  {3,&CCommand::DownLoadFile},
	  {4,&CCommand::MakeDeviceInfo},
	  {5,&CCommand::MakeDiretorInfo},
	  {6,&CCommand::MakeFileInfo},
	  {7,&CCommand::StartScreen},
	  {8,&CCommand::LockMachine},
	  {9,&CCommand::UnLockMachine},
	  {1981,&CCommand::testConnect},
	  {-1,nullptr}
	};

	for (int i = 0 ; i < sizeof(data)/sizeof(data[0]);i++)
	{
		this->m_funcMap.insert(std::pair<WORD,LPFUNC>(data[i].cmd,data[i].func));
	}
}


int CCommand::RunFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeDiretorInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::StartScreen(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::LockMachine(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}


int CCommand::UnLockMachine(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

CCommand::~CCommand()
{

}

int CCommand::ExecCommand(CPacket& packet,std::list<CPacket>& sendLst)
{
	std::map<WORD,LPFUNC>::iterator pr =  this->m_funcMap.find(packet.getCmd());
	if (pr == this->m_funcMap.end())//表示没有对应的命令号
	{
		return -1;
	}
	//将需要运行的对应的函数传递到线程池中
	ArgList arg(&packet,&sendLst);
	this->m_threadPool.DespatchWorker(this,(LPMEMWORKFUNC)pr->second,arg);
	return 0;
	//return (this->*pr->second)(packet,sendLst);
}