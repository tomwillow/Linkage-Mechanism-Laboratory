#pragma once
#include <stack>
#include "TTool.h"

class TElement;
class TSelectTool:public TTool
{
private:
	std::stack<int> PickedLineId,HoveredLineId;
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
	void TSelectTool::RestorePickedLineStyle();
	void TSelectTool::RestoreHoveredLineStyle();
public:
	TSelectTool();
	~TSelectTool();
	void TSelectTool::SelectById(int id);
};

