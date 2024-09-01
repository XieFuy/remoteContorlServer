#pragma once

#include<string>
class CFileInfo
{
public:
	std::string fileName; //文件名称
	std::string fileSize; //文件大小
	std::string fileType; //文件类型
	std::string fileAccessTime; //文件访问时间
	CFileInfo();
	~CFileInfo();
	void CountFileSize(ULONGLONG fileSize); //计算文件大小
	void ConvertFileAccessTime(const FILETIME& fileTime); //对文件访问时间进行转换
};

