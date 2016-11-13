#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>

#include "TWindow.h"

#pragma comment(lib, "comctl32.lib")

enum PartType { PT_FIXED, PT_NONE };

class TStatus
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
	HWND m_hParent;
	int m_iHeight;
public:
	HWND m_hWnd;
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
};

