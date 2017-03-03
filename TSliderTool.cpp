#pragma once

//#include "TCanvas.h"
#include "TAttach.h"

#include "TSliderTool.h"

TSliderTool::TSliderTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
}


TSliderTool::~TSliderTool()
{
	delete pAttach;
}

void TSliderTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}


void TSliderTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void TSliderTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	pAttach->AttachAll(ptPos);

}

void TSliderTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSliderTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSliderTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSliderTool::Draw(HDC hdc)
{
	pAttach->Draw(hdc);

	if (pAttach->bShowExtensionLine)
	{
		LOGPEN logpen = { PS_DOT, { 1, 0 }, pConfig->crPen };
		TDraw::DrawSliderRect(hdc, pConfig->RealToScreen(pAttach->dptAttach), pAttach->pAttachElement->angle,logpen);
	}
}
