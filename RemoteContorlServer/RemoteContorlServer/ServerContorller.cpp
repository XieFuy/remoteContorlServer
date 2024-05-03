#include "pch.h"
#include "ServerContorller.h"

 
CServerContorller::CServerContorller()
{
	TRACE("øÿ÷∆≤„“‘∆Ù∂Ø\r\n");
}

CServerContorller::~CServerContorller()
{

}

CServerContorller::CServerContorller(const CServerContorller& contorller)
{
	if (&contorller == this)
	{
		return;
	}
	
}

CServerContorller& CServerContorller::operator= (const CServerContorller& contorller)
{
	if (&contorller == this)
	{
		return *this;
	}
	return *this;
}

void CServerContorller::releaseInstance()
{
	if (CServerContorller::m_instance != nullptr)
	{
		delete CServerContorller::m_instance;
		CServerContorller::m_instance = nullptr;
	}
}

CServerContorller* CServerContorller::getInstance()
{
	if (CServerContorller::m_instance == nullptr)
	{
		CServerContorller::m_instance = new CServerContorller();
	}
	return CServerContorller::m_instance;
}

CServerContorller::CHelper CServerContorller::m_helper;
CServerContorller* CServerContorller::m_instance = nullptr;
