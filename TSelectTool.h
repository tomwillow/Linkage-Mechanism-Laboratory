#pragma once
#include "TTool.h"
class TSelectTool:public TTool
{
public:
	TSelectTool();
	~TSelectTool();
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
};

