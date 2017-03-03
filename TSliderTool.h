#pragma once
#include "TTool.h"

class TAttach;
class TSliderTool :
	public TTool
{
private:

	TAttach *pAttach;
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
};

