#pragma once
#include<Windows.h>
#include<map>
#include<list>
#include"Packet.h"
#include"ThreadPool.h"
#include<atlimage.h>
#include"MouseEvent.h"
#include"TestTool.h"
//������ ��������ִ�к���
class CCommand:public CThreadFuncBase
{
private:
	typedef int(CCommand::* LPFUNC)(CPacket&, std::list<CPacket>&);
	std::map<WORD, LPFUNC> m_funcMap;
	CThreadPool m_threadPool; //�̳߳أ���������Ͷ�ݵ��̳߳���ִ��
public:
	HANDLE m_signal; //�ź������̺߳�������ʱ֪ͨ���߳̿��Խ��з������ݰ�
	int ExecCommand(CPacket& packet,std::list<CPacket>& sendLst); //ִ������
	CCommand();
	~CCommand();
	int RunFile(CPacket& packet, std::list<CPacket>& sendLst); //�����ļ�  1
	int DelteRemoteFile(CPacket& packet, std::list<CPacket>& sendLst);//ɾ���ļ� 2
	int DownLoadFile(CPacket& packet, std::list<CPacket>& sendLst); //�����ļ� 3
	int MakeDeviceInfo(CPacket& packet, std::list<CPacket>& sendLst); //�鿴���̷��� 4
	int MakeDiretorInfo(CPacket& packet, std::list<CPacket>& sendLst);//��ȡ�ļ�����Ϣ 5
	int MakeFileInfo(CPacket& packet, std::list<CPacket>& sendLst); //��ȡ�ļ���Ϣ 6
	int StartScreen(CPacket& packet, std::list<CPacket>& sendLst); //��ػ��� 7
	int LockMachine(CPacket& packet, std::list<CPacket>& sendLst); //���� 8
	int UnLockMachine(CPacket& packet, std::list<CPacket>& sendLst); //���� 9
	int testConnect(CPacket& packet, std::list<CPacket>& sendLst);//�������� 1981
	int DealMouseEvent(CPacket& packet,std::list<CPacket>& sendLst); //����������
};

