#include "pch.h"
#include "Packet.h"

CPacket::CPacket()
{
	this->m_head = 0xFEFF;
	this->m_cmd = 0;
	this->m_dataLenght = 0;
	this->m_data = "";
	this->m_sum = 0;
}

CPacket::CPacket(WORD cmd, const BYTE* pData, size_t nDataSize)
{
	this->m_head = 0xFEFF;
	this->m_cmd = cmd;
	this->m_dataLenght = nDataSize + 4;
	this->m_sum = 0;
	if (pData != nullptr)
	{
		this->m_data.resize(nDataSize);
		memcpy((void*)this->m_data.c_str(),pData,nDataSize);

		//不计算和校验
		/*for (std::string::iterator pos = this->m_data.begin();pos != this->m_data.end();pos++)
		{
			this->m_sum += (((BYTE)(*pos)) & 0xFF);
		}*/
	}
}

CPacket::CPacket(const BYTE* pData, size_t& nSize)
{
	//寻找包头
	size_t i = 0;
	for (;i<nSize;i++)
	{
		if (*((WORD*)pData+i) == 0xFEFF)
		{		
			this->m_head = *((WORD*)(pData + i));
			i += 2;
			break;
		}
	}

	//判断包
	if (i+2+4+2 > nSize)
	{
		nSize = 0;
		return;
	}

	this->m_dataLenght = *((DWORD*)(pData + i));
	i += 4;

	if (this->m_dataLenght + i > nSize)
	{
		nSize = 0;
		return;
	}

	this->m_cmd = *((WORD*)(pData+i));
	i += 2;

	if (this->m_dataLenght> 4)
	{
		this->m_data.resize(this->m_dataLenght - 4);
		memcpy((void*)this->m_data.c_str(),pData+i,this->m_dataLenght-4);
		i += this->m_dataLenght - 4;
	}

	this->m_sum = *((WORD*)(pData + i));
	i += 2;
	
	/*WORD sum = 0;
	for (int j = 0 ; j < this->m_data.size();j++)
	{
		sum += (((BYTE)this->m_data.at(j)) & 0xFF);
	}
	if (this->m_sum == sum)
	{
		nSize = i;
		return;
	}
	nSize = 0;*/
	return;
}

CPacket& CPacket::operator=(const CPacket& packet)
{
	this->m_head = packet.m_head;
	this->m_cmd = packet.m_cmd;
	this->m_dataLenght = packet.m_dataLenght;
	this->m_data = packet.m_data;
	this->m_sum = packet.m_sum;
	return *this;
}

WORD CPacket::getCmd() 
{
	return this->m_cmd;
}

DWORD CPacket::getDataLenght()
{
	return this->m_dataLenght;
}

std::string& CPacket::getStrData()
{
	return this->m_data;
}

void CPacket::toByteData(std::string& data)
{
	data.resize(this->m_dataLenght + 6);
	char* pData = (char*)data.c_str();
	memcpy(pData,&this->m_head,sizeof(this->m_head));
	pData += 2;
	memcpy(pData,&this->m_dataLenght,sizeof(this->m_dataLenght));
	pData += 4;
	memcpy(pData,&this->m_cmd,sizeof(this->m_cmd));
	pData += 2;
	memcpy(pData,this->m_data.c_str(),this->m_data.size());
	pData += this->m_data.size();
	memcpy(pData, &this->m_sum, sizeof(this->m_sum));
	pData += 2;
	TRACE("进行转换后整个包的数据长度为: %d\r\n",data.size());
}