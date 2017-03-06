#pragma once
#include <vector>
#include "TTool.h"

class TSlider;
class TAttach;
class TSliderTool :
	public TTool
{
private:
	std::vector<POINT> dptHit;
	TAttach *pAttach;
	TSlider *pSlider;
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

