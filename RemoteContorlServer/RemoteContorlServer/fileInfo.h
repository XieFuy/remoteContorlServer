#pragma once

#include<string>
class CFileInfo
{
public:
	std::string fileName; //�ļ�����
	std::string fileSize; //�ļ���С
	std::string fileType; //�ļ�����
	std::string fileAccessTime; //�ļ�����ʱ��
	CFileInfo();
	~CFileInfo();
	void CountFileSize(ULONGLONG fileSize); //�����ļ���С
	void ConvertFileAccessTime(const FILETIME& fileTime); //���ļ�����ʱ�����ת��
};

