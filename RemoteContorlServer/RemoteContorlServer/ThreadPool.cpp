#include "pch.h"
#include "ThreadPool.h"

CThreadWorker::CThreadWorker()
{
	this->m_objetc = nullptr;
	this->m_workFunc = nullptr;
}

CThreadWorker::CThreadWorker(LPVOID object, LPMEMWORKFUNC m_workFunc)
{
	this->m_objetc = object;
	this->m_workFunc = m_workFunc;
}

CThreadWorker::CThreadWorker(const CThreadWorker& threadWorker)
{
	this->m_objetc = threadWorker.m_objetc;
	this->m_workFunc = threadWorker.m_workFunc;
}

CThreadWorker& CThreadWorker::operator=(const CThreadWorker& threadWorker)
{
	this->m_objetc = threadWorker.m_objetc;
	this->m_workFunc = threadWorker.m_workFunc;
	return *this;
}

BOOL CThreadWorker::isVaild()
{
	return (this->m_objetc != nullptr) && (this->m_workFunc != nullptr);
}

void CThreadWorker::setObject(LPVOID object)
{
	this->m_objetc = object;
}

void CThreadWorker::setWorkFunc(LPMEMWORKFUNC workFunc)
{
	this->m_workFunc = workFunc;
}

int CThreadWorker::operator()(CPacket& packet, std::list<CPacket>& sendLst)
{
	if (this->isVaild())
	{
		CThreadFuncBase* thiz = (CThreadFuncBase*)this->m_objetc;
	   return  (thiz->*this->m_workFunc)(packet,sendLst); //执行工作函数
	}
	return -1;
}

LPVOID CThreadWorker::getObject()
{
	return this->m_objetc;
}

LPMEMWORKFUNC CThreadWorker::getworkFunc()
{
	return this->m_workFunc;
}

CThread::CThread()
{
	this->m_worker = new CThreadWorker();
	this->m_thread = INVALID_HANDLE_VALUE;
	this->threadStatus = FALSE;
	this->m_arg.thiz = this;
}

CThread::~CThread()
{
	if (this->m_worker != nullptr)
	{
		delete this->m_worker;
		this->m_worker = nullptr;
	}
	CloseHandle(this->m_worker);
}
 
CThread::CThread(const CThread& thread){}

CThread& CThread::operator=(const CThread& thread) { return *this; }

BOOL CThread::isWorking()
{
	if (this->m_thread == nullptr || this->m_thread == INVALID_HANDLE_VALUE ||this->threadStatus == FALSE)
	{
		return FALSE;
	}
	return WaitForSingleObject(this->m_thread,0) == WAIT_TIMEOUT;
}

BOOL CThread::isIdel()
{
	if (this->m_worker == nullptr)
	{
		return TRUE;
	}
	return !this->m_worker->isVaild();
}

unsigned WINAPI CThread::threadEntry(LPVOID arg)
{
	ArgList* pArg = (ArgList*)arg;
	CThread* thiz = pArg->thiz;
	thiz->threadMain(*pArg->packet,*pArg->sendLst);
	thiz->threadStatus = FALSE;
	_endthreadex(0);
	return 0;
}

BOOL CThread::Start()
{
	if (this->m_thread == nullptr || this->m_thread == INVALID_HANDLE_VALUE)
	{
		if (this->m_worker->isVaild())
		{
			this->m_thread = (HANDLE)_beginthreadex(nullptr, 0, &CThread::threadEntry, &this->m_arg, 0, nullptr);
			this->threadStatus = TRUE;
			return TRUE;
		}	
	}
	return FALSE;
}

BOOL CThread::Stop()
{
	//等待线程任务结束后，进行回收资源
	if (WaitForSingleObject(this->m_thread,1000) == WAIT_TIMEOUT) //等待1秒后，强制结束线程
	{
		TerminateThread(this->m_thread,-1); //强制结束线程，可能会导致内存泄漏

		this->threadStatus = FALSE;
		this->m_worker->setObject(nullptr);
		this->m_worker->setWorkFunc(nullptr);
		return TRUE;
	}
	else if(WaitForSingleObject(this->m_thread,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(this->m_thread);
		this->m_thread = INVALID_HANDLE_VALUE;
		this->threadStatus = FALSE;
		this->m_worker->setObject(nullptr);
		this->m_worker->setWorkFunc(nullptr);
		return TRUE;
	}
	return  FALSE;
}

void CThread::threadMain(CPacket& packet, std::list<CPacket>& sendLst)
{
	//执行投递的任务
	((CThreadFuncBase*)this->m_worker->getObject()->*this->m_worker->getworkFunc())(packet,sendLst);
}

CThreadPool::CThreadPool()
{
	this->m_threadPool.resize(10);
}

CThreadPool::~CThreadPool(){}

CThreadPool::CThreadPool(const CThreadPool& threadPool){}

CThreadPool& CThreadPool::operator=(const CThreadPool& threadPool) { return *this; }

BOOL CThreadPool::StartPool() //开启线程池中的所有线程
{
	BOOL ret = TRUE;
	for (std::vector<CThread>::iterator pos = this->m_threadPool.begin();pos != this->m_threadPool.end();pos++)
	{
		if (pos->Start() == FALSE)
		{
			ret = FALSE;
			break;
		}
	}
	if (ret == FALSE)
	{
		this->StopPool();
	}
	return ret;
}

void CThreadPool::StopPool()
{
	for (std::vector<CThread>::iterator pos = this->m_threadPool.begin();pos != this->m_threadPool.end();pos++)
	{
		pos->Stop();
	}
}

BOOL CThreadPool::CheckThreadVaild(size_t index)
{
	if (this->m_threadPool.at(index).getWorker() == nullptr)
	{
		return FALSE;
	}
	return this->m_threadPool.at(index).getWorker()->isVaild();
}

CThreadWorker* CThread::getWorker()
{
	return this->m_worker;
}

ArgList& CThread::getArgList()
{
	return this->m_arg;
}

void CThreadPool::DespatchWorker(CThreadFuncBase* object, LPMEMWORKFUNC workFunc,ArgList arg)
{
	if (object == nullptr ||  workFunc == nullptr)
	{
		return;
	}
	for (std::vector<CThread>::iterator pos = this->m_threadPool.begin();pos != this->m_threadPool.end();pos++)
	{
		if (pos->isIdel())
		{
			pos->getArgList().thiz = &(*pos);
			pos->getArgList().packet = arg.packet;
			pos->getArgList().sendLst = arg.sendLst;
			pos->getWorker()->setObject(object);
			pos->getWorker()->setWorkFunc(workFunc);
			if (pos->Start() == FALSE)
			{
				this->StopPool();
			}
			else
			{
				//等待线程任务结束后进行释放线程资源
				while (pos->isWorking()){}
				pos->Stop();
			}
			break;
		}
	}
}

HANDLE CThread::getThreadHandle()
{
	return this->m_thread;
}