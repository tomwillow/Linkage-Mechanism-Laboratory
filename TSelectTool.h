#pragma once
#include <stack>
#include "TTool.h"
#include "String.h"

#include "DPOINT.h"

class TElement;
class TSelectTool:public TTool
{
protected:
	enum enumSelectMode{ SELECT_MOVE, SELECT_DRAG };
	enumSelectMode eMode;
protected:

	//String sTips;
	//bool bShowTips;
	//POINT ptTips;

	LPWSTR Cursor;

	bool bDrag;

	bool bMove;
	POINT ptMouseClick;

	int iPickIndex,iPrevPickIndex;
	int iHoverIndex;
	std::stack<int> PickedLineId,HoveredLineId;
	void TSelectTool::Draw(HDC hdc);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	bool TSelectTool::PickRealLine(POINT ptPos, TElement *Element);
	bool TSelectTool::PickRealLine(POINT &ptPos, DPOINT &dptBegin, DPOINT &dptEnd);
	bool TSelectTool::PickConstraintCoincide(POINT ptPos, TElement *element);
	void TSelectTool::CancelTreeViewAndListView();
	void TSelectTool::RestorePickedLineStyle();
	void TSelectTool::RestoreHoveredLineStyle();
	void TSelectTool::SelectNull();
	void TSelectTool::EndDrag();
	void TSelectTool::EndMove();
public:
	TSelectTool();
	~TSelectTool();
	bool TSelectTool::CanBeDriver();
	void TSelectTool::SelectById(int id);
};

