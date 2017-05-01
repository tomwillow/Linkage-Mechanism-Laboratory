#include "TConstraintColinearTool.h"

#include "TAttach.h"
#include "TConstraintColinear.h"

TConstraintColinearTool::TConstraintColinearTool()
{
	pColinear = NULL;

	pAttach = NULL;

	Reset();
}

void TConstraintColinearTool::Reset()
{
	if (pColinear != NULL) delete pColinear;
	pColinear = NULL;

	if (pAttach != NULL) delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	bShowTips = true;

	dptHit.clear();
}

TConstraintColinearTool::~TConstraintColinearTool()
{
	if (pColinear != NULL) delete pColinear;

	if (pAttach != NULL) delete pAttach;
}

void TConstraintColinearTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TConstraintColinearTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void TConstraintColinearTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachLine(ptPos);

	switch (dptHit.size())
	{
	case 0:
		if (pAttach->bShowExtensionLine)//已拾取
		{
			sTips = TEXT("已吸附：点击以确定第一个连接构件");
		}
		else
			sTips = TEXT("请选择要建立约束的第一个连接构件");

		break;
	case 1:
		if (pAttach->bShowExtensionLine)//已拾取
		{
		if (pAttach->pAttachElement == pColinear->pElement[0])//拾取自身
			sTips = TEXT("不能在同一个构件上建立约束");
		else
			sTips = TEXT("已吸附：点击以确定第二个连接构件");
		}
		else
			sTips = TEXT("请选择要建立约束的第二个连接构件");


		sTips += TEXT("\r\n右键/Esc 可取消");
		break;
	}

}


void TConstraintColinearTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (pAttach->bShowExtensionLine)//已拾取
	{
		dptHit.push_back(ptPos);
		switch (dptHit.size())
		{
		case 1://按下第一点
			pColinear = new TConstraintColinear;
			pColinear->SetStyle(pConfig->logpen);

			pColinear->pElement[0] = pAttach->pAttachElement;
			pColinear->PointBeginIndexOfElement[0] = pAttach->iAttachLinePointIndex[0];
			pColinear->PointEndIndexOfElement[0] = pAttach->iAttachLinePointIndex[1];


			break;
		case 2:
			if (pAttach->pAttachElement == pColinear->pElement[0])//拾取自身
			{
				sTips = TEXT("不能在同一个构件上建立约束");
				break;
			}

			pColinear->pElement[1] = pAttach->pAttachElement;
			pColinear->PointBeginIndexOfElement[1] = pAttach->iAttachLinePointIndex[0];
			pColinear->PointEndIndexOfElement[1] = pAttach->iAttachLinePointIndex[1];

			AddIntoShape();
			Reset();
			sTips = TEXT("建立完成");
			break;
		}
	}
	else//无效点击
		sTips = TEXT("选择无效：请点击已存在构件上的点");
}

void TConstraintColinearTool::AddIntoShape()
{
	//入库
	AddTreeViewItem(pColinear, pShape->iNextId);
	pShape->AddElement(pColinear);

	//刷新方程组
	RefreshEquations();
}

void TConstraintColinearTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
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

void TConstraintColinearTool::Draw(HDC hdc)
{

	if (pColinear != NULL)
	{
	POINT pt1 = pConfig->RealToScreen(TDraw::GetCenter(pColinear->pElement[0]->GetAbsolutePointByIndex(pColinear->PointBeginIndexOfElement[0]),
		pColinear->pElement[0]->GetAbsolutePointByIndex(pColinear->PointEndIndexOfElement[0])));
	POINT pt2 = pConfig->RealToScreen(pAttach->dptAttach);

		TDraw::DrawConstraintColinear_inner(hdc, pt1, pt2, pColinear->logpenStyleShow, pConfig);
	}

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}