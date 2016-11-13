#pragma once
#include <Windows.h>

//为避免重复刷新，工具类及其派生类不得发送WM_PAINT消息，刷新动作由Canvas完成。
class TTool
{
public:
	TTool();
	virtual ~TTool();
	virtual void OnKeyDown(HWND hWnd, WPARAM wParam,LPARAM lParam)   { }
	virtual void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void Draw(HDC hdc){}
};

