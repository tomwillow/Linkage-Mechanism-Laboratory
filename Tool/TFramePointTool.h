#pragma once
#include "TTool.h"

#include "..\Tool\TAttach.h"
#include "..\Element\TFramePoint.h"
#include "..\TConfiguration.h"

class TConfiguration;
class TFramePointTool:public TTool
{
private:
	TAttach *Attach;
	TFramePoint tempFramePoint;
	TElement *pPrevFramePoint;
public:
	TFramePointTool();
	~TFramePointTool();
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
	void TFramePointTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos);
	void TFramePointTool::Draw(HDC hdc);
};

