#pragma once

#include<string>
class CPacket
{
public:
	CPacket();
	CPacket(WORD cmd,const BYTE* pData,size_t nDataSize); //封包
	CPacket(const BYTE* pData,size_t& nSize); //解包
private:
	WORD m_head; //包头
	DWORD m_dataLenght; //包的数据和命令以及校验和的长度
	WORD m_cmd; //包的命令
	std::string m_data; //存储包数据
	WORD m_sum; //校验和
};

