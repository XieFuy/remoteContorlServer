#pragma once
#include<Windows.h>

//#pragma pack(push,1) //设置内存对齐为1，及成员之间不进行内存填充
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

