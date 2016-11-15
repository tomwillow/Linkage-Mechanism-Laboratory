#pragma once
#include <Windows.h>
#include <vector>

#include "TTool.h"
#include "DPOINT.h"

class TAttach;
class TLine;
class TRealLine;
class TLineEdit;
class TLineTool :public TTool
{
private:
	TAttach *Attach;
	std::vector<DPOINT> dptHit;//每次的点击位置
	TRealLine *MoveLine;//显示点的位置
	bool bShowDimLine;
	POINT pt1, pt2;
	TLine *Line1, *Line2, *LineDim;

	TLineEdit *LineEdit;
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos);
	void TLineTool::InitialLine(DPOINT dptPos);
	void Draw(HDC hdc);
protected:
	virtual void TLineTool::AddIntoShape(TRealLine &RealLine);
public:
	TLineTool();
	~TLineTool();
};

