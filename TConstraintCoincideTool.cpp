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
	pCoincide = NULL;

	if (pAttach != NULL) delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	bShowTips = true;

	dptHit.clear();
}

TConstraintCoincideTool::~TConstraintCoincideTool()
{
	if (pCoincide != NULL) delete pCoincide;

	if (pAttach != NULL) delete pAttach;
}

void TConstraintCoincideTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TConstraintCoincideTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		if (dptHit.empty())
			ResetTool();
		else
			Reset();
		break;
	case VK_SPACE:
	case VK_RETURN:
		break;
	}
}

void TConstraintCoincideTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachAll(ptPos);

	switch (dptHit.size())
	{
	case 0:
		if (pAttach->bAttachedEndpoint)//已拾取
		{
			sTips = TEXT("已吸附端点：点击以确定第一点");
		}
		else
		sTips = TEXT("请选择要建立约束的第一点");

		break;
	case 1:
		if (pAttach->bAttachedEndpoint)//已拾取
		{
			sTips = TEXT("已吸附端点：点击以确定第二点");
		}
		else
			sTips = TEXT("请选择要建立约束的第二点");

		if (pAttach->pAttachElement == pCoincide->pElement[0])//拾取自身
			sTips = TEXT("不能在同一个构件上建立约束");

		sTips += TEXT("\r\n右键/Esc 可取消");
		break;
	}

}

void TConstraintCoincideTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (pAttach->bAttachedEndpoint)//已拾取
	{
		dptHit.push_back(ptPos);
		switch (dptHit.size())
		{
		case 1://按下第一点
			pCoincide = new TConstraintCoincide;
			pCoincide->SetStyle(pConfig->logpen);

			pCoincide->pElement[0] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

			pCoincide->BuildpDpt_inner(0);

			break;
		case 2:
			if (pAttach->pAttachElement == pCoincide->pElement[0])//拾取自身
			{
				sTips = TEXT("不能在同一个构件上建立约束");
				break;
			}

			pCoincide->pElement[1] = pAttach->pAttachElement;
			pCoincide->PointIndexOfElement[1] = pAttach->iAttachElementPointIndex;

			AddIntoShape();
			Reset();
			sTips = TEXT("建立完成");
			break;
		}
	}
	else//无效点击
		sTips = TEXT("选择无效：请点击已存在构件上的点");
}

void TConstraintCoincideTool::AddIntoShape()
{
	//入库
	AddTreeViewItem(pCoincide, pShape->iNextId);
	pShape->AddElement(pCoincide);

	//刷新方程组
	RefreshEquations();
}

void TConstraintCoincideTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0:
		ResetTool();
		break;
	default:
	Reset();
	RefreshCanvas();
		break;
	}
}

void TConstraintCoincideTool::Draw(HDC hdc)
{
	if (pCoincide != NULL)
		TDraw::DrawConstraintCoincide(hdc, pCoincide->GetLinkDpt(0), pAttach->dptAttach, pCoincide->logpenStyleShow, pConfig);

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}
