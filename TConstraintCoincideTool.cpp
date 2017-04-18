#include "TConstraintCoincideTool.h"

#include "TAttach.h"

#include "TConstraintCoincide.h"

TConstraintCoincideTool::TConstraintCoincideTool()
{
	pCoincide = NULL;

	pAttach = NULL;

	Reset();
}

void TConstraintCoincideTool::Reset()
{
	if (pCoincide != NULL) delete pCoincide;

	if (pAttach != NULL) delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	bShowTips = true;
}

TConstraintCoincideTool::~TConstraintCoincideTool()
{
	if (pCoincide != NULL) delete pCoincide;

	if (pAttach != NULL) delete pAttach;
}

void TConstraintCoincideTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TConstraintCoincideTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void TConstraintCoincideTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachAll(ptPos);

	switch (dptHit.size())
	{
	case 0:
		sTips = TEXT("请选择要建立约束的点");
		break;
	}

	if (pAttach->bAttachedEndpoint)//已拾取
	{
		sTips = TEXT("已吸附端点：点击以确定第一点");
	}
}

void TConstraintCoincideTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (pAttach->bAttachedEndpoint)//已拾取
	{
		switch (dptHit.size())
		{
		case 0://按下第一点
			pCoincide = new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);

			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->BuildpDpt_inner(0);

			pCoincide->pDpt[1] = &(pAttach->dptAttach);
				break;
		}
	}
	else
		sTips = TEXT("选择无效：请点击已存在构件上的点");
}

void TConstraintCoincideTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TConstraintCoincideTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

void TConstraintCoincideTool::Draw(HDC hdc)
{
	if (pCoincide != NULL)
		TDraw::DrawConstraintCoincide(hdc, pCoincide, pConfig);

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}
