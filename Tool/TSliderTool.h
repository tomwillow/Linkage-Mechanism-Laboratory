#pragma once
#include <vector>
#include <stack>
#include "TTool.h"
#include "..\Common\String.h"

class TConstraintColinear;
class TConstraintCoincide;
class TSlider;
class TAttach;
class TSliderTool :
	public TTool
{
private:
	std::vector<POINT> dptHit;
	TAttach *pAttach;
	TSlider *pSlider;

	std::stack<TConstraintColinear *>stackpColinear;
	std::stack<TConstraintCoincide *>stackpCoincide;
	void TSliderTool::AddIntoShape();
public:
	TSliderTool();
	~TSliderTool();
	void TSliderTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void TSliderTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TSliderTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void TSliderTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TSliderTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TSliderTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos);
	void TSliderTool::Draw(HDC hdc);
	void TSliderTool::Reset();
};

