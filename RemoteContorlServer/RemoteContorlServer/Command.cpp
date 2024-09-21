#include "pch.h"
#include "Command.h"

FILE* CCommand::m_pFile = nullptr;
long long CCommand::alreadySend = 0;
long long CCommand::lenght = 0;

int CCommand::testConnect(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("���ԣ��ͻ������ӳɹ�!\r\n");
	sendLst.push_back(CPacket(1981,nullptr,0));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::DealMouseEvent(CPacket &packet, std::list<CPacket>& sendLst)
{   
	CMouseEvent mouseEvent;
	CTestTool::Dump((const BYTE*)packet.getStrData().c_str(), packet.getStrData().size());
	memcpy(&mouseEvent,packet.getStrData().c_str(),packet.getStrData().size());
	TRACE("isLeftBtn:%d isRight:%d isLeftBtnPress:%d isRightBtnPress:%d isLeftUp: %d isRightUp:%d isMoving:%d\n",mouseEvent.isLeftBtn,
		mouseEvent.isRightBtn,mouseEvent.isLeftBtnPress,mouseEvent.isRightBtnPress,
		mouseEvent.isLeftBtnUp,mouseEvent.isRightBtnUp,mouseEvent.isMoveing);

	//�ж��Ƿ�������ƶ�
	if (mouseEvent.isMoveing)
	{
		::SetCursorPos(mouseEvent.x,mouseEvent.y);
	}
	if (mouseEvent.isLeftBtn)  //���
	{
		if (mouseEvent.isLeftBtnPress) //�������
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
		if (mouseEvent.isLeftBtnUp) //�������
		{
			mouse_event(MOUSEEVENTF_LEFTUP,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
	}
	if (mouseEvent.isRightBtn)//�Ҽ�
	{
		if (mouseEvent.isRightBtnPress)//�Ҽ�����
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
		if (mouseEvent.isRightBtnUp) //�Ҽ�����
		{
			mouse_event(MOUSEEVENTF_RIGHTUP,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
	}
	sendLst.push_back(CPacket(10,nullptr,0));
	SetEvent(this->m_signalMouseEvent);
	return packet.getCmd();
}

CCommand::CCommand()
{
	this->m_signal = CreateEvent(nullptr,FALSE,FALSE,nullptr);
	this->m_signalMouseEvent = CreateEvent(nullptr,FALSE,FALSE,nullptr);
	//���ع��ܺ�����map��
	struct 
	{
		WORD cmd;
		LPFUNC func;
	}data[]{
	  {1,&CCommand::RunFile},
	  {2,&CCommand::DelteRemoteFile},
	  {3,&CCommand::DownLoadFile},
	  {4,&CCommand::MakeDeviceInfo},
	  {5,&CCommand::MakeDiretorInfo},
	  {6,&CCommand::MakeFileInfo},
	  {7,&CCommand::StartScreen},
	  {8,&CCommand::LockMachine},
	  {9,&CCommand::UnLockMachine},
	  {1981,&CCommand::testConnect},
	  {10,&CCommand::DealMouseEvent},
	  {-1,nullptr}
	};

	for (int i = 0 ; i < sizeof(data)/sizeof(data[0]);i++)
	{
		this->m_funcMap.insert(std::pair<WORD,LPFUNC>(data[i].cmd,data[i].func));
	}
}


int CCommand::RunFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("�����ļ��ѱ�ִ�У����͹�����·����Ϣ��%s\r\n",packet.getStrData().c_str());
	//�����ֽ�תΪ���ֽ�
	int len = MultiByteToWideChar(CP_UTF8, 0, packet.getStrData().c_str(), -1, nullptr, 0);
	std::wstring wstr(len, '\0');
	MultiByteToWideChar(CP_UTF8, 0, packet.getStrData().c_str(), -1, &wstr[0], len);
	HINSTANCE ret =  ShellExecuteW(nullptr,nullptr,wstr.data(),nullptr,nullptr,SW_SHOWNORMAL);
	sendLst.push_back(CPacket(1,nullptr,0));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	int len = MultiByteToWideChar(CP_UTF8,0, packet.getStrData().c_str(),-1,nullptr,0);
	std::wstring str(len, '\0');
	MultiByteToWideChar(CP_UTF8,0, packet.getStrData().c_str(),-1,&str[0],len);
	DeleteFileW(str.data());
	sendLst.push_back(CPacket(2,nullptr,0));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("���ص��ļ�Ϊ��%s\r\n", packet.getStrData().c_str());

	//�ж��ַ����Ƿ����#�ţ�������������ļ��Ĵ�С
	if (packet.getStrData().at(packet.getStrData().size() - 1 ) == '#')
	{
		packet.getStrData().pop_back();
		TRACE("ȥ�����־Ϊ��%s\r\n", packet.getStrData().c_str());

		CCommand::alreadySend = 0; //��ʾ�Ѿ����͵��ֽ���
		CCommand::lenght = 0;
		CCommand::m_pFile = nullptr;

		//��������ʹ�ÿ��ֽ��������ļ���д
		int len = MultiByteToWideChar(CP_UTF8, 0, packet.getStrData().c_str(), -1, nullptr, 0);
		std::wstring str(len, '\0');
		MultiByteToWideChar(CP_UTF8, 0, packet.getStrData().c_str(), -1, &str[0], len);

		//�򿪸��ļ������ҽ��ļ��Ĵ�С���͸��ͻ���
		CCommand::m_pFile = _wfopen(str.data(), L"rb");
		if (CCommand::m_pFile == nullptr)
		{
			TRACE("���ļ�ʧ�ܣ�%s[%d]��%s  Error:%d\r\n", __FILE__, __LINE__, __FUNCTION__, errno);
			sendLst.push_back(CPacket(100, nullptr, 0));
			SetEvent(this->m_signal);
			return -1;
		}
		//���м����ļ��Ĵ�С
		fseek(CCommand::m_pFile, 0, SEEK_END);
		CCommand::lenght = _ftelli64(CCommand::m_pFile);
		fseek(CCommand::m_pFile, 0, SEEK_SET);
		char buffer[sizeof(long long)] = { 0 };
		memcpy(buffer, &CCommand::lenght, sizeof(CCommand::lenght));
		sendLst.push_back(CPacket(3, (const BYTE*)buffer, strlen(buffer)));
		SetEvent(this->m_signal);
		return packet.getCmd();
	}
	else  //���ж�ȡ�ļ������ҽ���ȡ�����ݷ��͵��ͻ���
	{
		//�����ļ��ϵ���������
		//char bufferSize[102400] = { 0 }; //ÿ�η����ļ����ݵĴ�С
		char* bufferSize = new char[307200];
		if(alreadySend < lenght)
		{
			memset(bufferSize, 0, 307200);
			size_t size = fread(bufferSize, 1, 307200, CCommand::m_pFile);
			alreadySend += size;
			sendLst.push_back(CPacket(101, (const BYTE*)bufferSize, size));
			delete[] bufferSize;
			SetEvent(this->m_signal);
			return packet.getCmd();
		}
		else
		{
			fclose(CCommand::m_pFile);
			CCommand::alreadySend = 0;
			CCommand::lenght = 0;
		}
	}
	return packet.getCmd();
}

int CCommand::MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("����˽��յ��ͻ��˵����ݰ�!");
	std::string diskInfoStr = "";
	for (int i = 1 ; i <= 26 ; i++)
	{
		if (_chdrive(i) == 0)
		{
			diskInfoStr.push_back('A' + i - 1);
		}
	}
	TRACE("������Ϣ�� %s\r\n",diskInfoStr);
	sendLst.push_back(CPacket (4, (const BYTE*)diskInfoStr.c_str(), diskInfoStr.size()));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::MakeDiretorInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("�ļ���Ϣ��ȡ�����ִ�У� ���յ����ļ�·��Ϊ��%s\r\n",packet.getStrData().c_str());
	//���б�����ǰ·���µ������ļ����ļ���
	char path[MAX_PATH] = {0};
	memcpy(path, packet.getStrData().c_str(),packet.getStrData().size());
	strcat(path,"*");
	TRACE("���յ���·����Ϣ��%s\r\n",path);
	
	//תΪ���ֽ�
	int len = MultiByteToWideChar(CP_UTF8,0,path,-1,nullptr,0);
	std::wstring wPath(len,'\0');
	MultiByteToWideChar(CP_UTF8,0,path,-1,&wPath[0],len);


	WIN32_FIND_DATAW hFindData;
	HANDLE hFindFile =  FindFirstFileW(wPath.data(),&hFindData);
	do
	{
		//��ÿһ���ļ���Ϣ���з��͵��ͻ�����
		if (lstrcmpW(hFindData.cFileName,L".") == 0 || lstrcmpW(hFindData.cFileName,L"..")==0)
		{
			continue;
		}
		CFileInfo* fileInfo = new CFileInfo();

		std::wstring temp = hFindData.cFileName; //��ס std::string תΪstd::wstringʹ�õ���CP_UTF8  std::wstring תΪstd::string ʹ�õ���CP_CAP
		OutputDebugStringW(temp.data());
		len = WideCharToMultiByte(CP_ACP, 0, &temp[0], temp.size(), nullptr, 0, nullptr, nullptr);
		std::string str(len, '\0');
		WideCharToMultiByte(CP_ACP, 0, &temp[0], temp.size(), &str[0], len, nullptr, nullptr);
		TRACE("ת������ļ���Ϊ: %s\r\n",str.c_str());

		fileInfo->fileName = str;
		//fileInfo->fileName = hFindData.cFileName;
		if (hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //����ļ��������ļ���
		{
			fileInfo->fileType = "�ļ���";
		}
		else//Ϊ��ͨ�ļ�
		{
			fileInfo->fileType = "�ļ�";
		}

		if (fileInfo->fileType == "�ļ�")
		{
			ULARGE_INTEGER fileSize;
			fileSize.LowPart = hFindData.nFileSizeLow;
			fileSize.HighPart = hFindData.nFileSizeHigh;
			ULONGLONG sizeInBytes = fileSize.QuadPart;
			fileInfo->CountFileSize(sizeInBytes);
		}	
		fileInfo->ConvertFileAccessTime(hFindData.ftLastAccessTime);		

		//���ṹ��������л���Ȼ����з�������
		std::string sendData = "";
		sendData += fileInfo->fileName;
		sendData += "#";
		sendData += fileInfo->fileSize;
		sendData += "#";
		sendData += fileInfo->fileType;
		sendData += "#";
		sendData += fileInfo->fileAccessTime;
		sendData += "#";
		TRACE("���͵��ַ���Ϊ��%s\r\n",sendData.c_str());
			
		CPacket packet(6,(const BYTE*)sendData.c_str(),sendData.size());
		sendLst.push_back(packet);
		TRACE("���Դ�ӡ��ǰ�ļ���%s �Ĵ�СΪ��%s �ļ�����Ϊ��%s ���·���ʱ�䣺%s\r\n",fileInfo->fileName.c_str(),fileInfo->fileSize.c_str(),fileInfo->fileType.c_str(),fileInfo->fileAccessTime.c_str());
		delete fileInfo;
		fileInfo = nullptr;
	} while (FindNextFileW(hFindFile,&hFindData));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::StartScreen(CPacket& packet, std::list<CPacket>& sendLst)
{
	//����һ��ͼƬ���Ͷ���
	CImage screen;
	//��ȡ��Ļ�豸��������
	HDC screenDC = ::GetDC(NULL);
	//��ȡ��Ļ�����ظ���
	int nPerpixel = GetDeviceCaps(screenDC, BITSPIXEL);
	//��ȡ�豸��Ļ��������
	int nWidht = GetDeviceCaps(screenDC, HORZRES);
	//��ȡ�豸��Ļ������߶�
	int nHeight = GetDeviceCaps(screenDC, VERTRES);
	//��������ߴ��CImage����
	screen.Create(nWidht, nHeight, nPerpixel, 0);
	//����ǰ�豸�Ľ�ͼ���ݽ��п�����CImage������ȥ
	BitBlt(screen.GetDC(), 0, 0, nWidht, nHeight, screenDC, 0, 0, SRCCOPY);
	//�ͷŻ�ȡ������Ļ��������
	ReleaseDC(NULL, screenDC);

	// ��������ȫ�ֵĳ��ȿɱ�Ķ��ڴ�
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL)
	{
		return -1;
	}
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream); //����һ��������
	if (ret == S_OK)  //�����ɹ�
	{
		/*��ͼƬ���ݱ�������������*/
		screen.Save(pStream, Gdiplus::ImageFormatPNG); //����������ΪPNG��ʽ
		LARGE_INTEGER bg = { 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);//����������ָ�����õ��ͷ
		PBYTE pData = (PBYTE)GlobalLock(hMem);
		SIZE_T nSize = GlobalSize(hMem);

		/*�����ݷ�װ�ɰ���ת�����ͻ���*/

		sendLst.push_back(CPacket(7, pData, nSize));
		/*	��ȫ�ֶ��ڴ���н���*/
		GlobalUnlock(hMem);
	}
	/*�ͷ�������*/
	pStream->Release();
	/*����ȫ�ֶ��ڴ���ͷ�*/
	GlobalFree(hMem);
	//��CImage����������Ľ����ͷ�
	screen.ReleaseDC();
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::LockMachine(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::UnLockMachine(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

CCommand::~CCommand()
{

}

int CCommand::ExecCommand(CPacket& packet,std::list<CPacket>& sendLst)
{
	std::map<WORD,LPFUNC>::iterator pr =  this->m_funcMap.find(packet.getCmd());
	if (pr == this->m_funcMap.end())//��ʾû�ж�Ӧ�������
	{
		return -1;
	}
	//����Ҫ���еĶ�Ӧ�ĺ������ݵ��̳߳���
	ArgList arg(&packet,&sendLst);
	this->m_threadPool.DespatchWorker(this,(LPMEMWORKFUNC)pr->second,arg);
	return 0;
	//return (this->*pr->second)(packet,sendLst);
}

int CCommand::ExecCommandMouseEvent(CPacket& packet, std::list<CPacket>& sendLst)
{
	std::map<WORD, LPFUNC>::iterator pr = this->m_funcMap.find(packet.getCmd());
	if (pr == this->m_funcMap.end())//��ʾû�ж�Ӧ�������
	{
		return -1;
	}
	//����Ҫ���еĶ�Ӧ�ĺ������ݵ��̳߳���
	ArgList arg(&packet, &sendLst);
	this->m_threadPoolMouseEvent.DespatchWorker(this, (LPMEMWORKFUNC)pr->second, arg);
	return 0;
}