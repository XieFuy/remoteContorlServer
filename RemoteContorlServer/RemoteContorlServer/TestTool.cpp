#include "pch.h"
#include "TestTool.h"

void CTestTool::Dump(const BYTE* Data, size_t nSize)  //��ӡ���������Ƶİ���������ʲô
{
	std::string strOut;  //���ڴ洢�����������ݵĽ��
	//strOut.resize(nSize);
	for (size_t i = 0; i < nSize; i++)
	{
		char buf[8] = "";
		if (i > 0 && (i % 16 == 0))
		{
			strOut += '\n';
		}
		snprintf(buf, sizeof(buf), "%02X", Data[i] & 0xFF);  //%02X����˼�� д���ʮ������ռ��λ������λ�����ǰ���� 
		strOut += buf;
	}
	strOut += "\n";
	OutputDebugStringA(strOut.c_str()); //����������Ե��ַ�����������QDebug
}
