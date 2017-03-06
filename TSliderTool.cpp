#pragma once
#include "TSlider.h"

//#include "TCanvas.h"
#include "TAttach.h"

#include "TSliderTool.h"

TSliderTool::TSliderTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
	pSlider = NULL;
}


TSliderTool::~TSliderTool()
{
	delete pAttach;
	if (pSlider != NULL)
		delete pSlider;
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
	dptHit.push_back(ptPos);

	switch (dptHit.size())
	{
	case 1://第一次点，设置滑块
		if (pAttach->bShowExtensionLine)//已拾取直线
		{
			//设置临时直线约束

			//设置临时滑块
			pSlider = new TSlider;
		}
		else
		{

		}
		break;
	case 2:
		break;
	}
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
		TDraw::DrawSliderRect(hdc, pConfig->RealToScreen(pAttach->dptAttach), pAttach->pAttachElement->angle, logpen);
	}
}
