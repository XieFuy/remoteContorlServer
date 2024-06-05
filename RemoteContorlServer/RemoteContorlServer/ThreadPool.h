#pragma once
#include"Packet.h"
#include<list>
#include<vector>
class CThread;

typedef struct ArgList  //�̲߳����б�ṹ��
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
	LPVOID m_objetc; //����
	LPMEMWORKFUNC m_workFunc;//��������
public:
	int operator()(CPacket& packet, std::list<CPacket>& sendLst); //����������������
	CThreadWorker();
	CThreadWorker(LPVOID object, LPMEMWORKFUNC m_workFunc);
	CThreadWorker(const CThreadWorker& threadWorker);
	CThreadWorker& operator=(const CThreadWorker& threadWorker);
	BOOL isVaild();//�жϹ��������Ƿ���Ч
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
	BOOL threadStatus; //�߳��Ƿ���� false���߳̽��� trueΪ�߳���������
public:
	CThread();
	~CThread();
	CThread(const CThread& thread);
	CThread& operator=(const CThread& thread);
	BOOL isWorking(); //�ж��߳��Ƿ����ڹ���  
	//void UpdateWorker(CThreadWorker* worker); //���Ĺ�������
	BOOL isIdel(); //�ж��߳��Ƿ����
	BOOL Start(); //�����߳�  
	BOOL Stop(); //���������߳�
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
	BOOL StartPool(); //�����̳߳�
	void StopPool(); //��ͣȫ���̳߳�
	void DespatchWorker(CThreadFuncBase* object,LPMEMWORKFUNC workFunc,ArgList arg); //����������Ͷ�뵽�̳߳���
	BOOL CheckThreadVaild(size_t index); //�ж��߳��Ƿ���Ч��worker��Աָ�벻Ϊ�գ�
};