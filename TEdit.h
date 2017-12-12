#pragma once
#include <Windows.h>
#include "tchar_head.h"
#include <stdio.h>

#include "TControl.h"
class TEdit :public TControl
{
private:
protected:
	//虚拟消息处理函数，可重载
	virtual LRESULT TEdit::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	//接收VK_RETURN,VK_ESCAPE等，返回是则调用默认消息处理
	virtual bool TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam){ return true; }

	//接收字符按键，返回是则调用默认消息处理
	virtual bool TEdit::OnChar(WPARAM wParam, LPARAM lParam){ return true; }

	//接收失去焦点消息，返回是则调用默认消息处理
	virtual bool TEdit::OnKillFocus(WPARAM wParam, LPARAM lParam){ return true; }
public:
	//bool bSendParentUserMsg=false;//如果置true,则接收到WM_KEYDOWN时向父窗口发送WM_USER消息
	bool bMultiLine,bAutoHScrol,bAutoVScrol,bNoHideSel;
	TEdit();
	~TEdit();
	void TEdit::CreateEditEx(HWND hParent, UINT id, HINSTANCE hInst, DWORD dwStyle = WS_EX_CLIENTEDGE);//创建Edit
	void TEdit::SetSelect(int iStart, int iEnd);//选择
	void TEdit::SetSelectAll();//全选
};