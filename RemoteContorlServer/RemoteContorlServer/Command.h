#pragma once

#include<Windows.h>
#include<map>
#include<list>
#include"Packet.h"
#include"ThreadPool.h"
#include<atlimage.h>
#include"MouseEvent.h"
#include"TestTool.h"
#include<direct.h>
#include"fileInfo.h"
#include<string.h>
#include "ServerSocket.h"

//命令类 包含命令执行函数
class CCommand:public CThreadFuncBase
{
private:
	typedef int(CCommand::* LPFUNC)(CPacket&, std::list<CPacket>&);
	std::map<WORD, LPFUNC> m_funcMap;
	CThreadPool m_threadPool; //线程池，将任务函数投递到线程池中执行
	CThreadPool m_threadPoolMouseEvent;
	static FILE* m_pFile ;
 	static long long lenght ;
	static long long alreadySend ;
	static std::wstring updataFilePath;
	static int count;
public:
	static char* packet;
	static size_t nSize; //一个数据包的大小
	HANDLE m_signal; //信号量，线程函数结束时通知主线程可以进行发送数据包
	HANDLE m_signalMouseEvent;
	int ExecCommand(CPacket& packet,std::list<CPacket>& sendLst); //执行命令
	int ExecCommandMouseEvent(CPacket& packet, std::list<CPacket>& sendLst);
	CCommand();
	~CCommand();
	int RunFile(CPacket& packet, std::list<CPacket>& sendLst); //运行文件  1
	int DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst);//删除文件 2
	int DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst); //下载文件 3
	int MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst); //查看磁盘分区 4
	int UpdataFile(CPacket& packet, std::list<CPacket>& sendLst);//将文件发送到客户端 5
	int MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst); //获取文件信息 6
	int StartScreen(CPacket& packet, std::list<CPacket>& sendLst); //监控画面 7
	int LockMachine(CPacket& packet, std::list<CPacket>& sendLst); //锁机 8
	int UnLockMachine(CPacket& packet, std::list<CPacket>& sendLst); //解锁 9
	int testConnect(CPacket& packet, std::list<CPacket>& sendLst);//测试连接 1981
	int DealMouseEvent(CPacket& packet,std::list<CPacket>& sendLst); //鼠标操作处理
};

