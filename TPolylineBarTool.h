#pragma once
#include <vector>
#include <stack>
#include "TTool.h"
#include "DPOINT.h"

class TConstraintCoincide;
class TConstraintColinear;
class TPolylineBar;
class TAttach;
class TPolylineBarTool :
	public TTool
{
private:
	std::vector<DPOINT> dptHit;
	TAttach *pAttach;
	TPolylineBar *pPolylineBar;

	std::stack<TConstraintColinear *>stackpColinear;
	std::vector<TConstraintCoincide *>vecpCoincide;
	void TPolylineBarTool::AddIntoShape();
	void TPolylineBarTool::Reset();
public:
	TPolylineBarTool();
	~TPolylineBarTool();
	void TPolylineBarTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void TPolylineBarTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void TPolylineBarTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TPolylineBarTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TPolylineBarTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TPolylineBarTool::Draw(HDC hdc);
};

