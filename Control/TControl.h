#pragma once
#include <Windows.h>
#include "..\Common\String.h"

class TControl
{
private:
	HFONT m_hFont;
	static LRESULT CALLBACK TControl::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	//LONG m_iWidth, m_iHeight;
	HWND m_hParent;
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TControl();
	~TControl();
	void TControl::LinkControl(HWND hwnd);
	void TControl::LinkControl(HWND hDlg, int id);
	RECT TControl::GetClientRect();
	void TControl::SetRect(RECT &rect);
	void TControl::SetRect(int x1, int y1, int x2, int y2); 
	//void TControl::SetPos(RECT &rect);
	void TControl::Invalidate();
	void TControl::SetFont(HFONT hFont);
	void TControl::SetDefaultGuiFont();
	void TControl::SetFont(TCHAR FontName[], int FontSize);
	void TControl::SetText(const String &s);
	void CDECL TControl::SetText(const TCHAR szFormat[], ...);//设置内容
	void TControl::GetText(TCHAR text[]);
	TCHAR * TControl::GetText();
	int TControl::GetLength();//获取字符串长度	
	void TControl::SetPosition(int x, int y, int width, int height);//设置大小及位置
	void TControl::SetPosition(RECT rect);//设置大小及位置
	void TControl::SetPositionOnlyOrigin(const RECT &rect);
	void TControl::SetVisible(bool bShow);//设置可见性
	bool TControl::GetVisible();
	void TControl::SetDouble(double d);
	double TControl::GetDouble();
	void TControl::SetEnable(bool bEnable);
	bool TControl::GetEnable();
protected:
	TCHAR *Text;
	void TControl::RegisterProc();//创建窗口后注册

	virtual LRESULT TControl::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//虚拟消息处理函数，可覆盖

};
