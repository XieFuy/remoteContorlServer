#pragma once
#include"Packet.h"
#include<list>
#include<vector>
class CThread;

typedef struct ArgList  //线程参数列表结构体
{
public:
	CThread* thiz;
	CPacket* packet;
	std::list<CPacket>* sendLst;

	ArgList()
	{
		this->thiz = nullptr;
		this->packet = nullptr;
		this->sendLst = nullptr;
	}
	
	ArgList(CPacket* packet, std::list<CPacket>* sendLst)
	{
		this->packet = packet;
		this->sendLst = sendLst;
	}
}ARGLIST;

class CThreadFuncBase {};

typedef int(CThreadFuncBase::* LPMEMWORKFUNC)(CPacket&, std::list<CPacket>&);
class CThreadWorker
{
private:	
	LPVOID m_objetc; //对象
	LPMEMWORKFUNC m_workFunc;//工作函数
public:
	int operator()(CPacket& packet, std::list<CPacket>& sendLst); //工作函数进行运行
	CThreadWorker();
	CThreadWorker(LPVOID object, LPMEMWORKFUNC m_workFunc);
	CThreadWorker(const CThreadWorker& threadWorker);
	CThreadWorker& operator=(const CThreadWorker& threadWorker);
	BOOL isVaild();//判断工作对象是否有效
	void setObject(LPVOID object);
	void setWorkFunc(LPMEMWORKFUNC workFunc);
	LPVOID getObject();
	LPMEMWORKFUNC getworkFunc();
};

class CThread 
{
private:
	ArgList m_arg;
	CThreadWorker* m_worker;
	HANDLE m_thread;
	static unsigned WINAPI threadEntry(LPVOID arg);
	void threadMain(CPacket& packet,std::list<CPacket>& sendLst);
	BOOL threadStatus; //线程是否结束 false则线程结束 true为线程正在运行
public:
	CThread();
	~CThread();
	CThread(const CThread& thread);
	CThread& operator=(const CThread& thread);
	BOOL isWorking(); //判断线程是否正在工作  
	//void UpdateWorker(CThreadWorker* worker); //更改工作对象
	BOOL isIdel(); //判断线程是否空闲
	BOOL Start(); //开启线程  
	BOOL Stop(); //结束销毁线程
	CThreadWorker* getWorker();
	ArgList& getArgList();
	HANDLE getThreadHandle();
};

class CThreadPool
{
private:
	std::vector<CThread> m_threadPool;
public:
	CThreadPool();
	~CThreadPool();
	CThreadPool(const CThreadPool& threadPool);
	CThreadPool& operator=(const CThreadPool& threadPool);
	BOOL StartPool(); //启动线程池
	void StopPool(); //暂停全部线程池
	void DespatchWorker(CThreadFuncBase* object,LPMEMWORKFUNC workFunc,ArgList arg); //将工作函数投入到线程池中
	BOOL CheckThreadVaild(size_t index); //判断线程是否有效（worker成员指针不为空）
};