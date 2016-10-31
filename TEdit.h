#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

class TEdit
{
protected:
	static WNDPROC oldEditProc;
	static LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam); 
	void TEdit::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle,int x, int y, int nWidth, int nHeight, HWND hParent,HMENU hMenu, HINSTANCE hInst);

	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//接收VK_RETURN,VK_ESCAPE，返回值决定是否调用默认消息处理
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam);

	//接收字符按键，返回值决定是否调用默认消息处理
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam);
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst);//创建Edit
	void TEdit::SetPos(int x, int y, int width, int height);//设置大小及位置
	void TEdit::ShowWindow(bool bShow);//设置可见性
	void CDECL TEdit::SetText(TCHAR szFormat[], ...);//设置内容
	int TEdit::GetLength();//获取字符串长度
	void TEdit::SetSelect(int iStart, int iEnd);//选择
	void TEdit::SetSelectAll();//全选
};

