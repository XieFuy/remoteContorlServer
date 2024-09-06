#include "pch.h"
#include "fileInfo.h"

CFileInfo::CFileInfo()
{
	this->fileName = "";
	this->fileSize = "";
	this->fileType = "";
	this->fileAccessTime = "";
}

CFileInfo::~CFileInfo()
{

}

void CFileInfo::CountFileSize(ULONGLONG fileSize)
{
	if (fileSize < 1024) //1k以内
	{
		this->fileSize = std::to_string(static_cast<unsigned long long>(fileSize));
		this->fileSize += "B";
	}
	else if (fileSize < (1024 * 1024)) //1MB以内
	{
		int ret = fileSize / 1024;
		this->fileSize = std::to_string(ret);
		this->fileSize += "KB";
	}
	else if (fileSize < (1024 * 1024 * 1024)) //1GB以内
	{
		int ret = fileSize / (1024 * 1024);
		this->fileSize = std::to_string(ret);
		this->fileSize += "MB";
	}
	else  //1GB以上
	{
		int ret = fileSize / (1024 * 1024 * 1024);
		this->fileSize = std::to_string(ret);
		this->fileSize += "GB";
	}
}

void CFileInfo::ConvertFileAccessTime(const FILETIME& fileTime)
{
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&fileTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);	

	TRACE(L"Access Time: %02d/%02d/%d %02d:%02d:%02d\n",
		stLocal.wDay, stLocal.wMonth, stLocal.wYear,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

	std::string year =  std::to_string(stLocal.wYear);
	std::string month = std::to_string(stLocal.wMonth);
	std::string day = std::to_string(stLocal.wDay);
	std::string hour = std::to_string(stLocal.wHour);
	std::string minute = std::to_string(stLocal.wMinute);
	std::string second = std::to_string(stLocal.wSecond);
 
	this->fileAccessTime  += year;
	this->fileAccessTime += "-";
	this->fileAccessTime += month;
	this->fileAccessTime += "-";
	this->fileAccessTime += day;
	//this->fileAccessTime += "-";
	this->fileAccessTime += "  ";
	this->fileAccessTime += hour;
	this->fileAccessTime += ":";
	this->fileAccessTime += minute;
	this->fileAccessTime += ":";
	this->fileAccessTime += second;
	TRACE("转为字符串后的时间：%s\r\n", this->fileAccessTime.data());
}


