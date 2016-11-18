#pragma once
#include "Resource.h"
#include "TFramePointTool.h"
#include "TCanvas.h"

#include "TTreeViewContent.h"
#include "TConstraintCoincide.h"

TFramePointTool::TFramePointTool()
{
	Attach = new TAttach(pCanvas->m_hWnd, pShape, pConfig);

	tempFramePoint.SetStyle(PS_SOLID, 1, pConfig->crPen);
}


TFramePointTool::~TFramePointTool()
{
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
	pShape->AddFramePoint(tempFramePoint); 

	if (Attach->iAttachElementId != -1)
	{
		TConstraintCoincide coincide;
		coincide.eElementType1 = Attach->eAttachElementType;
		coincide.ElementId1 = Attach->iAttachElementId;
		coincide.Element1PointIndex = Attach->iAttachElementPointIndex;

		coincide.eElementType2 = ELEMENT_FRAMEPOINT;
		coincide.ElementId2 = pShape->iNextId-1;
		coincide.Element2PointIndex = 1;

		pTreeViewContent->AddItem(&coincide, pShape->iNextId);
		pShape->AddCoincide(coincide);
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