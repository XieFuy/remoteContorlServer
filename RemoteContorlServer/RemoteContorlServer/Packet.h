#pragma once

#include<string>
class CPacket
{
public:
	CPacket();
	CPacket(WORD cmd,const BYTE* pData,size_t nDataSize); //���
	CPacket(const BYTE* pData,size_t& nSize); //���
private:
	WORD m_head; //��ͷ
	DWORD m_dataLenght; //�������ݺ������Լ�У��͵ĳ���
	WORD m_cmd; //��������
	std::string m_data; //�洢������
	WORD m_sum; //У���
};

