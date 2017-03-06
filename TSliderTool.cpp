#pragma once
#include "TSlider.h"

#include "TCanvas.h"
#include "resource.h"
#include "TAttach.h"

#include "TSliderTool.h"

TSliderTool::TSliderTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
	pSlider = NULL;


	pSlider = new TSlider;
	pSlider->SetStyle(pConfig->logpen);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);
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

	switch (dptHit.size())
	{
	case 0:
		//设置临时滑块
		pSlider->dpt = pAttach->dptAttach;
		if (pAttach->bShowExtensionLine)//已拾取直线
		{
			pSlider->angle = pAttach->pAttachElement->angle;
		}
		else
		{
			pSlider->angle = 0.0;
		}
		break;
	case 1://已有块位置
		//设置线位置
		
		pSlider->vecDpt[1] =TDraw::GetRelative(pAttach->dptAttach,pSlider->dpt,pSlider->angle);
		pSlider->vecLine[0] = { 0, 1 };

		break;
	}
}

void TSliderTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	dptHit.push_back(ptPos);

	switch (dptHit.size())
	{
	case 1://第一次点，设置滑块

		//设置临时滑块
		pSlider->dpt = pAttach->dptAttach;
		pSlider->vecDpt[0] = TDraw::GetRelative(pSlider->dpt, pSlider->dpt, pSlider->angle);
		pSlider->vecDpt.resize(2);
		pSlider->vecIsJoint.resize(2);
		pSlider->vecLine.resize(1);
		if (pAttach->bShowExtensionLine)//已拾取直线
		{
			//设置临时直线约束

			pSlider->angle = pAttach->pAttachElement->angle;
		}
		else
		{
			pSlider->angle = 0.0;
		}
		break;
	case 2://第二次点 入库

		//滑块入库
		pShape->AddSlider(pSlider);

		//共线约束入库

		if (pAttach->bAttachedEndpoint)//捕捉到端点 则添加重合约束
		{
			//添加重合约束

			//重合约束入库
		}

		//重置状态
		Reset();
		OnMouseMove(hWnd, nFlags, ptPos);

		break;
	}
}

void TSliderTool::Reset()
{
	//重设临时块
	if (pSlider!=NULL)
		delete pSlider;
	pSlider = new TSlider;
	pSlider->SetStyle(pConfig->logpen);
	pSlider->vecDpt.resize(1);
	pSlider->vecIsJoint.resize(1);

	//重设状态
	dptHit.clear();

	//delete pAttach;
	//pAttach = new TAttach(pCanvas, pShape, pConfig);

}

void TSliderTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0://空点右键
		//没有点的情况下点右键则重置工具
		::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
		break;
	case 1://刚画滑块还没画线

		//线设置为原点
		pSlider->vecDpt.resize(1);
		pSlider->vecIsJoint.resize(1);
		pSlider->vecLine.clear();

		//滑块入库
		pShape->AddSlider(pSlider);

		//共线约束入库

		//重置状态
		Reset();
		OnMouseMove(hWnd, nFlags, ptPos);

		pCanvas->Invalidate();

		break;
	}
}

void TSliderTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TSliderTool::Draw(HDC hdc)
{

	TDraw::DrawSlider(hdc, pSlider, pConfig);

	pAttach->Draw(hdc);
}
