#include "pch.h"
#include "Command.h"


int CCommand::testConnect(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("测试：客户端连接成功!\r\n");
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

	//判断是否是鼠标移动
	if (mouseEvent.isMoveing)
	{
		::SetCursorPos(mouseEvent.x,mouseEvent.y);
	}
	if (mouseEvent.isLeftBtn)  //左键
	{
		if (mouseEvent.isLeftBtnPress) //左键按下
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
		if (mouseEvent.isLeftBtnUp) //左键弹起
		{
			mouse_event(MOUSEEVENTF_LEFTUP,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
	}
	if (mouseEvent.isRightBtn)//右键
	{
		if (mouseEvent.isRightBtnPress)//右键按下
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN,mouseEvent.x,mouseEvent.y,0,GetMessageExtraInfo());
		}
		if (mouseEvent.isRightBtnUp) //右键弹起
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
	//加载功能函数到map中
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
	TRACE("运行文件已被执行！传送过来的路径信息：%s\r\n",packet.getStrData().c_str());
	//将单字节转为多字节
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
	return packet.getCmd();
}

int CCommand::MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	TRACE("服务端接收到客户端的数据包!");
	std::string diskInfoStr = "";
	for (int i = 1 ; i <= 26 ; i++)
	{
		if (_chdrive(i) == 0)
		{
			diskInfoStr.push_back('A' + i - 1);
		}
	}
	TRACE("磁盘信息： %s\r\n",diskInfoStr);
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
	TRACE("文件信息获取服务端执行！ 接收到的文件路径为：%s\r\n",packet.getStrData().c_str());
	//进行遍历当前路径下的所有文件和文件夹
	char path[MAX_PATH] = {0};
	memcpy(path, packet.getStrData().c_str(),packet.getStrData().size());
	strcat(path,"*");
	WIN32_FIND_DATA hFindData;
	HANDLE hFindFile =   FindFirstFile(path,&hFindData);
	do
	{
		//将每一个文件信息进行发送到客户端中
		if (lstrcmp(hFindData.cFileName,".") == 0 || lstrcmp(hFindData.cFileName,"..")==0)
		{
			continue;
		}
		CFileInfo* fileInfo = new CFileInfo();
		fileInfo->fileName = hFindData.cFileName;
		if (hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //如果文件类型是文件夹
		{
			fileInfo->fileType = "文件夹";
		}
		else//为普通文件
		{
			fileInfo->fileType = "文件";
		}

		if (fileInfo->fileType == "文件")
		{
			ULARGE_INTEGER fileSize;
			fileSize.LowPart = hFindData.nFileSizeLow;
			fileSize.HighPart = hFindData.nFileSizeHigh;
			ULONGLONG sizeInBytes = fileSize.QuadPart;
			fileInfo->CountFileSize(sizeInBytes);
		}	
		fileInfo->ConvertFileAccessTime(hFindData.ftLastAccessTime);		

		//将结构体进行序列化，然后进行发包传输
		std::string sendData = "";
		sendData += fileInfo->fileName;
		sendData += "#";
		sendData += fileInfo->fileSize;
		sendData += "#";
		sendData += fileInfo->fileType;
		sendData += "#";
		sendData += fileInfo->fileAccessTime;
		sendData += "#";
		TRACE("发送的字符串为：%s\r\n",sendData.c_str());
		CPacket packet(6,(const BYTE*)sendData.c_str(),sendData.size());
		sendLst.push_back(packet);
		TRACE("测试打印当前文件：%s 的大小为：%s 文件类型为：%s 最新访问时间：%s\r\n",fileInfo->fileName.c_str(),fileInfo->fileSize.c_str(),fileInfo->fileType.c_str(),fileInfo->fileAccessTime.c_str());
		delete fileInfo;
		fileInfo = nullptr;
	} while (FindNextFile(hFindFile,&hFindData));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

int CCommand::StartScreen(CPacket& packet, std::list<CPacket>& sendLst)
{
	//创建一个图片类型对象
	CImage screen;
	//获取屏幕设备的上下文
	HDC screenDC = ::GetDC(NULL);
	//获取屏幕的像素个数
	int nPerpixel = GetDeviceCaps(screenDC, BITSPIXEL);
	//获取设备屏幕的物理宽度
	int nWidht = GetDeviceCaps(screenDC, HORZRES);
	//获取设备屏幕的物理高度
	int nHeight = GetDeviceCaps(screenDC, VERTRES);
	//创建这个尺寸的CImage对象
	screen.Create(nWidht, nHeight, nPerpixel, 0);
	//将当前设备的截图内容进行拷贝到CImage对象中去
	BitBlt(screen.GetDC(), 0, 0, nWidht, nHeight, screenDC, 0, 0, SRCCOPY);
	//释放获取到的屏幕的上下文
	ReleaseDC(NULL, screenDC);

	// 进行申请全局的长度可变的堆内存
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL)
	{
		return -1;
	}
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream); //创建一个流对象
	if (ret == S_OK)  //创建成功
	{
		/*将图片数据保存在输入流中*/
		screen.Save(pStream, Gdiplus::ImageFormatPNG); //这里是设置为PNG格式
		LARGE_INTEGER bg = { 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);//将输入流的指针设置到最开头
		PBYTE pData = (PBYTE)GlobalLock(hMem);
		SIZE_T nSize = GlobalSize(hMem);

		/*将数据封装成包，转发给客户端*/

		sendLst.push_back(CPacket(7, pData, nSize));
		/*	对全局堆内存进行解锁*/
		GlobalUnlock(hMem);
	}
	/*释放流对象*/
	pStream->Release();
	/*进行全局堆内存的释放*/
	GlobalFree(hMem);
	//对CImage对象的上下文进行释放
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
	if (pr == this->m_funcMap.end())//表示没有对应的命令号
	{
		return -1;
	}
	//将需要运行的对应的函数传递到线程池中
	ArgList arg(&packet,&sendLst);
	this->m_threadPool.DespatchWorker(this,(LPMEMWORKFUNC)pr->second,arg);
	return 0;
	//return (this->*pr->second)(packet,sendLst);
}

int CCommand::ExecCommandMouseEvent(CPacket& packet, std::list<CPacket>& sendLst)
{
	std::map<WORD, LPFUNC>::iterator pr = this->m_funcMap.find(packet.getCmd());
	if (pr == this->m_funcMap.end())//表示没有对应的命令号
	{
		return -1;
	}
	//将需要运行的对应的函数传递到线程池中
	ArgList arg(&packet, &sendLst);
	this->m_threadPoolMouseEvent.DespatchWorker(this, (LPMEMWORKFUNC)pr->second, arg);
	return 0;
}