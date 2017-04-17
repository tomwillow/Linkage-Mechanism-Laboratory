
#include "TPolylineBarTool.h"

#include "TAttach.h"

#include "TPolylineBar.h"
#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"

TPolylineBarTool::TPolylineBarTool()
{
	pPolylineBar = NULL;
	pAttach = NULL;

	Reset();
}


TPolylineBarTool::~TPolylineBarTool()
{
	delete pAttach;

	if (pPolylineBar != NULL)
		delete pPolylineBar;
}

void TPolylineBarTool::Reset()
{
	//重设临时块
	if (pPolylineBar != NULL)
		delete pPolylineBar;
	pPolylineBar = new TPolylineBar;
	pPolylineBar->SetStyle(pConfig);

	//重设状态
	dptHit.clear();

	bShowTips = true;
	sTips = TEXT("点击以建立多段杆");

	if (pAttach != NULL)
		delete pAttach;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	RefreshCanvas();
}

void TPolylineBarTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TPolylineBarTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	if (dptHit.empty())
		pAttach->AttachAll(ptPos);
	else
		pAttach->AttachAll(ptPos, dptHit.back());

	switch (dptHit.size())
	{
	case 0:
		break;
	default:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		break;
	}
}

void TPolylineBarTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0:
		pPolylineBar->dpt = pAttach->dptAttach;
		pPolylineBar->angle = 0;
		pPolylineBar->vecDpt.push_back({ 0, 0 });
		pPolylineBar->vecDpt.push_back({ 0, 0 });
		sTips = TEXT("右键/Esc可取消绘制");
		break;
	default:
		DPOINT dptRelative = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		pPolylineBar->vecDpt.back() = dptRelative;
		pPolylineBar->vecDpt.push_back(dptRelative);
		sTips = TEXT("右键/Enter/空格可完成绘制\r\nEsc可取消绘制");
		break;
	}
	pAttach->vecdpt.push_back(pAttach->dptAttach);

	dptHit.push_back(pAttach->dptAttach);
}

void TPolylineBarTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void TPolylineBarTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	switch (dptHit.size())
	{
	case 0://切换工具
		ResetTool();
	case 1:
		Reset();
		break;
	default:
		AddIntoShape();
		break;
	}
}

void TPolylineBarTool::AddIntoShape()
{

	//入库
	AddTreeViewItem(pPolylineBar, pShape->iNextId);
	TPolylineBar *pSavedPolylineBar = pShape->AddElement(pPolylineBar);

	//重合约束入库
	while (!stackpCoincide.empty())
	{
		stackpCoincide.top()->pElement[1] = pSavedPolylineBar;

		AddTreeViewItem(stackpCoincide.top(), pShape->iNextId);
		pShape->AddElement(stackpCoincide.top());

		delete stackpCoincide.top();
		stackpCoincide.pop();
	}

	//共线约束入库
	while (!stackpColinear.empty())
	{
		stackpColinear.top()->pElement[1] = pSavedPolylineBar;

		AddTreeViewItem(stackpColinear.top(), pShape->iNextId);
		pShape->AddElement(stackpColinear.top());

		delete stackpColinear.top();
		stackpColinear.pop();
	}

	//刷新方程组
	RefreshEquations();
}

void TPolylineBarTool::Draw(HDC hdc)
{
	TDraw::DrawPolylineBar(hdc,pPolylineBar, pConfig);

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}