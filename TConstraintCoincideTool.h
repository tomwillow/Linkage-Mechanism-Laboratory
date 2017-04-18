#pragma once
#include <vector>
#include "TTool.h"

class TAttach;
class TConstraintCoincide;
class TConstraintCoincideTool :
	public TTool
{
private:
	std::vector<POINT> dptHit;
	TAttach *pAttach;
	TConstraintCoincide *pCoincide;
public:
	TConstraintCoincideTool();
	~TConstraintCoincideTool();
	void TConstraintCoincideTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintCoincideTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TConstraintCoincideTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintCoincideTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintCoincideTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintCoincideTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintCoincideTool::Draw(HDC hdc);
	void TConstraintCoincideTool::Reset();
};

