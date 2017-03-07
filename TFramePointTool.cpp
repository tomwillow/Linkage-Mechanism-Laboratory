#pragma once
#include "Resource.h"
#include "TFramePointTool.h"
#include "TCanvas.h"

#include "TTreeViewContent.h"
#include "TConstraintCoincide.h"
#include "TSolver.h"

TFramePointTool::TFramePointTool()
{
	Attach = new TAttach(pCanvas, pShape, pConfig);

	tempFramePoint.SetStyle(PS_SOLID, 1, pConfig->crPen);

	pPrevFramePoint = NULL;
}


TFramePointTool::~TFramePointTool()
{
	delete Attach;
}


void TFramePointTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos);
	tempFramePoint.dpt = Attach->dptAttach;
}


void TFramePointTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	tempFramePoint.dpt = Attach->dptAttach;

	pTreeViewContent->AddItem(&tempFramePoint, pShape->iNextId);
	pPrevFramePoint=pShape->AddFramePoint(tempFramePoint);

	if (Attach->bAttachedEndpoint)
	{
		TConstraintCoincide coincide;
		coincide.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);

		coincide.pElement[0] = Attach->pAttachElement;
		coincide.PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

		coincide.pElement[1] = pPrevFramePoint;
		coincide.PointIndexOfElement[1] = 0;

		pTreeViewContent->AddItem(&coincide, pShape->iNextId);

		pShape->AddCoincide(coincide, pConfig);//约束入库

		pSolver->RefreshEquations(true);
	}

}

void TFramePointTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
}

//插入WM_PAINT事件中进行绘制
void TFramePointTool::Draw(HDC hdc)
{
	Attach->Draw(hdc);

	//已存储点由Canvas负责绘制
}

void TFramePointTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}