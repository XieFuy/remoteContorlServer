#pragma once
#include<Windows.h>

//#pragma pack(push,1) //�����ڴ����Ϊ1������Ա֮�䲻�����ڴ����
class CMouseEvent
{
public:
	CMouseEvent();
	~CMouseEvent();
	BOOL isLeftBtn;
	BOOL isRightBtn;
	BOOL isLeftBtnPress;
	BOOL isRightBtnPress;
	BOOL isLeftBtnUp;
	BOOL isRightBtnUp;
	BOOL isMoveing;
	int x;
	int y;
};
//#pragma pack(pop)

