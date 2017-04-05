#pragma once

#include "tchar_head.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>

#include "TControl.h"

#pragma comment(lib, "comctl32.lib")

enum PartType { PT_FIXED,//固定大小
	PT_NONE };//可伸缩

class TStatus:public TControl
{
private:
	int flex_num;
	int has_used_width;
	PartType *m_pePartsType;
	UINT m_iPartsNum;
	int *m_piPartsWidth;
	int *m_piPartsRightPos;
	int *m_piID;
	RECT *m_prcPartsRect;
	int m_iHeight;
public:
	UINT m_ControlID;
	TStatus();
	~TStatus();
	void TStatus::Create(HWND hParent, UINT nControlID, HINSTANCE hInst, int iHeight);
	RECT TStatus::GetClientRect();
	RECT TStatus::GetPartRect(int id, int margin);
	void TStatus::FreshSize();
	void TStatus::SetText(int pane, LPCTSTR str);
	void CDECL TStatus::SetText(int id, TCHAR szFormat[], ...);
	void TStatus::AddPart(int id, int iWidth, PartType type);
	LRESULT TStatus::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//虚拟消息处理函数，可重载
	{
		switch (uMsg)
		{
		case WM_COMMAND:
			::PostMessage(m_hParent, WM_COMMAND, wParam, lParam);
			break;
		}
		return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
	}
};

