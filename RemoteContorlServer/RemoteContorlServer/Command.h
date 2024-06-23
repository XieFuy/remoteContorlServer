#pragma once
#include<Windows.h>
#include<map>
#include<list>
#include"Packet.h"
#include"ThreadPool.h"
#include<atlimage.h>
#include"MouseEvent.h"
#include"TestTool.h"
//命令类 包含命令执行函数
class CCommand:public CThreadFuncBase
{
private:
	typedef int(CCommand::* LPFUNC)(CPacket&, std::list<CPacket>&);
	std::map<WORD, LPFUNC> m_funcMap;
	CThreadPool m_threadPool; //线程池，将任务函数投递到线程池中执行
public:
	HANDLE m_signal; //信号量，线程函数结束时通知主线程可以进行发送数据包
	int ExecCommand(CPacket& packet,std::list<CPacket>& sendLst); //执行命令
	CCommand();
	~CCommand();
	int RunFile(CPacket& packet, std::list<CPacket>& sendLst); //运行文件  1
	int DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst);//删除文件 2
	int DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst); //下载文件 3
	int MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst); //查看磁盘分区 4
	int MakeDiretorInfo(CPacket& packet, std::list<CPacket>& sendLst);//获取文件夹信息 5
	int MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst); //获取文件信息 6
	int StartScreen(CPacket& packet, std::list<CPacket>& sendLst); //监控画面 7
	int LockMachine(CPacket& packet, std::list<CPacket>& sendLst); //锁机 8
	int UnLockMachine(CPacket& packet, std::list<CPacket>& sendLst); //解锁 9
	int testConnect(CPacket& packet, std::list<CPacket>& sendLst);//测试连接 1981
	int DealMouseEvent(CPacket& packet,std::list<CPacket>& sendLst); //鼠标操作处理
};

