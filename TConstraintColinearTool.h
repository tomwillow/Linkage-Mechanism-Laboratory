#pragma once
#include <vector>
#include "TTool.h"

class TAttach;
class TConstraintColinear;
class TConstraintColinearTool :
	public TTool
{
private:
	std::vector<POINT> dptHit;
	TAttach *pAttach;
	TConstraintColinear *pColinear;
	void TConstraintColinearTool::Reset();
	void TConstraintColinearTool::AddIntoShape();
public:
	TConstraintColinearTool();
	~TConstraintColinearTool();
	void TConstraintColinearTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintColinearTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TConstraintColinearTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintColinearTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintColinearTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TConstraintColinearTool::Draw(HDC hdc);
};