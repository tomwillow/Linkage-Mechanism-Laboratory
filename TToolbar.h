#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#pragma comment(lib,"comctl32.lib")//样式使用

class TToolbar
{
private:
	HINSTANCE m_hInst;
	int iButtonNum;
	TBBUTTON *tbButtons; 
	void TToolbar::CreateImageList(UINT uMsg, int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::AddElement(int BitmapIndex, int idCommand, BYTE fsState, BYTE fsStyle, BYTE bReverse[], DWORD_PTR dwData, INT_PTR iString); 
public:
	bool bIsFlat;//浮动样式
	bool bTextOnRight;//文本放置在右边
	bool bAutoWrap;//按钮自动换行
	HWND m_hWnd;//工具栏句柄
	TToolbar();
	~TToolbar();
	void TToolbar::CreateToolbar(HWND hwndParent, HINSTANCE hInst);
	void TToolbar::LoadImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::LoadHotImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::LoadDisabledImageList(int cx, int cy, UINT BitmapID, COLORREF crMask);
	void TToolbar::AddButton(int IconIndex, int idCommand, bool Enable, TCHAR iString[]); 
	void TToolbar::AddGroup(int IconIndex, int idCommand, bool Enable, TCHAR iString[]);
	void TToolbar::AddCheck(int IconIndex, int idCommand, bool Enable, TCHAR iString[]);
	void TToolbar::AddSeparator(int iWidth);
	void TToolbar::ShowToolbar();
	void TToolbar::FreshSize();
	RECT TToolbar::GetClientRect();
};

