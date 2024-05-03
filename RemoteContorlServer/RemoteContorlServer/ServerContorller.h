#pragma once

//¿ØÖÆ²ã 
class CServerContorller
{
private:
	static CServerContorller* m_instance;
	CServerContorller();
	~CServerContorller();
	CServerContorller(const CServerContorller& contorller);
	CServerContorller& operator= (const CServerContorller& contorller);
	static void releaseInstance();
	class CHelper 
	{
	public:
		CHelper()
		{
			CServerContorller::getInstance();
		}
		~CHelper() 
		{
			CServerContorller::releaseInstance();
		}
	};
	static CHelper m_helper;
public:
	static CServerContorller* getInstance();
};

