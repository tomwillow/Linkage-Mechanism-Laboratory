#pragma once
#include "TFramePointTool.h"

#include "TMainWindow.h"

extern TMainWindow win;
TFramePointTool::TFramePointTool()
{
	Attach = new TAttach(win.Canvas.m_hWnd, &(win.m_Shape), &(win.m_Configuration));
	Config = &(win.m_Configuration);

	tempFramePoint.SetStyle(PS_SOLID, 1, Config->crPen);
}


TFramePointTool::~TFramePointTool()
{
}


void TFramePointTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos);
	tempFramePoint.dpt = Attach->dptAttach;
}


void TFramePointTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	tempFramePoint.dpt = Attach->dptAttach;
	win.m_Shape.AddFramePoint(tempFramePoint);
}

void TFramePointTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::PostMessage(win.m_hWnd, WM_COMMAND, ID_SELECT, 0);
}

//插入WM_PAINT事件中进行绘制
void TFramePointTool::Draw(HDC hdc)
{
	Attach->Draw(hdc);

	//已存储点由Canvas负责绘制
}

void TFramePointTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}