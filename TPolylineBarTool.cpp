
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
	Reset();

	if (pAttach != NULL)
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


	while (!vecpCoincide.empty())
	{
		delete vecpCoincide.back();
		vecpCoincide.pop_back();
	}

	while (!stackpColinear.empty())
	{
		delete stackpColinear.top();
		stackpColinear.pop();
	}

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
	sTips = TEXT("点击以建立多段杆");
	break;
	case 1:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		sTips = TEXT("右键/Esc可取消绘制");
		break;
	default:
		pPolylineBar->vecDpt.back() = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);

		sTips = TEXT("右键/Enter/空格可完成绘制\r\nEsc可取消绘制");
		break;
	}

	if (pAttach->bAttachedEndpoint)//捕捉到端点
	{
		sTips += TEXT("\r\n已吸附端点：建立重合约束");
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
		break;
	default:
		DPOINT dptRelative = TDraw::GetRelative(pAttach->dptAttach, dptHit[0], pPolylineBar->angle);
		pPolylineBar->vecDpt.back() = dptRelative;
		pPolylineBar->vecDpt.push_back(dptRelative);
		break;
	}
	pPolylineBar->vecIsJoint.resize(pPolylineBar->vecDpt.size());//设置Joint容器大小

	pAttach->vecdpt.push_back(pAttach->dptAttach);//同步加入Attach中用以吸附


	if (pAttach->bAttachedEndpoint)
	{
		TConstraintCoincide *pCoincide = new TConstraintCoincide;
		pCoincide->SetStyle(pConfig->logpen);

		pCoincide->pElement[0] = pAttach->pAttachElement;
		pCoincide->PointIndexOfElement[0] = pAttach->iAttachElementPointIndex;

		pCoincide->pElement[1] = pPolylineBar;
		pCoincide->PointIndexOfElement[1] = dptHit.size();

		vecpCoincide.push_back(pCoincide);
	}

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
		pPolylineBar->vecDpt.erase(pPolylineBar->vecDpt.end() - 1);//去掉最后一个
		pPolylineBar->vecIsJoint.resize(pPolylineBar->vecDpt.size());

		AddIntoShape();
		Reset();
		break;
	}
}

void TPolylineBarTool::AddIntoShape()
{

	//入库
	AddTreeViewItem(pPolylineBar, pShape->iNextId);

	TPolylineBar *pSavedPolylineBar = dynamic_cast<TPolylineBar *>(pShape->AddElement(pPolylineBar));

	//重合约束入库
	while (!vecpCoincide.empty())
	{
		vecpCoincide.back()->pElement[1] = pSavedPolylineBar;//更新重合约束链接到刚保存的元素

		AddTreeViewItem(vecpCoincide.back(), pShape->iNextId);
		pShape->AddElement(vecpCoincide.back());

		delete vecpCoincide.back();
		vecpCoincide.pop_back();
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

	for (auto pCoincide : vecpCoincide)
	{
		pCoincide->BuildpDpt();
		TDraw::DrawConstraintCoincide(hdc, pCoincide, pConfig);
	}

	pAttach->Draw(hdc);

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}