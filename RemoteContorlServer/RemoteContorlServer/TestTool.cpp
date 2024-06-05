#include "pch.h"
#include "TestTool.h"

void CTestTool::Dump(const BYTE* Data, size_t nSize)  //打印输出测试设计的包的数据是什么
{
	std::string strOut;  //用于存储整个包的数据的结果
	//strOut.resize(nSize);
	for (size_t i = 0; i < nSize; i++)
	{
		char buf[8] = "";
		if (i > 0 && (i % 16 == 0))
		{
			strOut += '\n';
		}
		snprintf(buf, sizeof(buf), "%02X", Data[i] & 0xFF);  //%02X的意思是 写入的十六进制占两位，不足位是填充前导零 
		strOut += buf;
	}
	strOut += "\n";
	OutputDebugStringA(strOut.c_str()); //用于输出调试的字符串，类似于QDebug
}
