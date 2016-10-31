#include "TSelectTool.h"


TSelectTool::TSelectTool()
{
}


TSelectTool::~TSelectTool()
{
}


void TSelectTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}


void TSelectTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}