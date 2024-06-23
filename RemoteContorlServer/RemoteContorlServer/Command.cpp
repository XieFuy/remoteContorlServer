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
	sendLst.push_back(CPacket(10,nullptr,0));
	SetEvent(this->m_signal);
	return packet.getCmd();
}

CCommand::CCommand()
{
	this->m_signal = CreateEvent(nullptr,FALSE,FALSE,nullptr);
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
	return packet.getCmd();
}

int CCommand::DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeDiretorInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	return packet.getCmd();
}

int CCommand::MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst)
{
	 
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