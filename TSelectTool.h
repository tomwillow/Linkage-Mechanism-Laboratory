#pragma once
#include "TTool.h"

class TElement;
class TSelectTool:public TTool
{
private:
	int iPickIndex;
	int iHoverIndex;
	void TSelectTool::Draw(HDC hdc);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	bool TSelectTool::PickRealLine(POINT ptPos, TElement *Element);
	void TSelectTool::CancelTreeViewAndListView();
public:
	TSelectTool();
	~TSelectTool();
	void TSelectTool::SelectById(int id);
};

