#include "pch.h"
#include "MouseEvent.h"

CMouseEvent::CMouseEvent()
{
	this->isLeftBtn = FALSE;
	this->isRightBtn = FALSE;
	this->isLeftBtnPress = FALSE;
	this->isRightBtnPress = FALSE;
	this->isLeftBtnUp = FALSE;
	this->isRightBtnUp = FALSE;
	this->isMoveing = FALSE;
	this->x = 0;
	this->y = 0;
}

CMouseEvent::~CMouseEvent()
{

}