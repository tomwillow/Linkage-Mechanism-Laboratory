#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include <algorithm>
#include "..\resource.h"

#include "..\Tool\TSelectTool.h"
#include "..\Tool\TAttach.h"

#include "..\Element\TSlider.h"
#include "..\Control\TCanvas.h"
#include "..\Element\TShape.h"
#include "..\Control\TListView.h"
#include "..\Control\TTreeViewContent.h"

#include "..\Draw\TDraw.h"
#include "..\TConfiguration.h"

#include "..\Element\TFramePoint.h"
#include "..\Element\TRealLine.h"
#include "..\Element\TConstraintCoincide.h"

#include "..\Draw\TDrawTranslucent.h"

#include "..\Solver\TSolver.h"

using std::vector;

TSelectTool::TSelectTool()
{
	eMode = SELECT_MOVE;
	//bShowTips = false;

	Cursor = IDC_ARROW;

	bShowTips = true;
	bShowSelRect = false;

	bSelCross = false;

	eState = NONE;
	pAttach = new TAttach(pCanvas, pShape, pConfig);

	RegisterHotKey(pCanvas->m_hWnd, 1, MOD_CONTROL, 'A');
}

//由TTool的虚析构函数重载
TSelectTool::~TSelectTool()
{
	SelectNull();
}


void TSelectTool::SelectNull()
{
	CancelTreeViewAndListView();

	for (auto pElement : PickedElements)
		pElement->SetStateNormal();
	PickedElements.clear();
}

void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
	{
		//清除属性栏
		pListView->DeleteAllItems();

		//清理树状图
		for (auto pElement : PickedElements)
			pTreeViewContent->DeleteById(pElement->id);

		//删除选中元素
		std::vector<int> InfluencedId;
		InfluencedId=pShape->Delete(PickedElements.self());

		for (auto i : InfluencedId)
			pTreeViewContent->DeleteById(i);

		//刷新方程
		pSolver->RefreshEquations();

		PickedElements.clear();

		pCanvas->Invalidate();
	}
	return;
	}
}

void TSelectTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, Cursor));
}


void TSelectTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;

	if ((nFlags & MK_LBUTTON) > 0)//按住左键
	{
		switch (eState)
		{
		case MOVE_SEL:
		{
			//pAttach->AttachAll(ptPos);
			//ptMouse = pConfig->RealToScreen(pAttach->dptAttach);

			DPOINT dptDelta = pConfig->ScreenToReal(ptPos) - pConfig->ScreenToReal(ptDrag);

			for (auto pElement : PickedElements)
				pElement->ChangePos(dptDelta);

			ptDrag = ptPos;

			sTips = TEXT("移动到新位置以放置元件");
			Cursor = IDC_HAND;
			break;
		}
		case DRAG_SEL://拖动求解
		{
			pSolver->ClearOutput();
			pSolver->ClearConstraint();
			pSolver->AddMouseConstraint(PickedElements.front(), pConfig->ScreenToReal(ptPos));
			pSolver->Solve();
			pShape->SimplifyPhiValue();

			if (PickedElements.front()->CanBeDragged())
				sTips = TEXT("可拖动");
			else
				sTips = TEXT("该构件不可拖动");
			Cursor = IDC_HAND;

			break;
		}
		case NONE:
		{
			if (eMode == SELECT_MOVE)
			{
				bShowSelRect = true;
				rcSelect.right = ptPos.x;
				rcSelect.bottom = ptPos.y;

				for (auto pElement : pShape->Element)
				{
					if (pElement->InSelect(rcSelect, bSelCross, pConfig))
					{
						pElement->SetStateHover();//改变框选元素样式
					}
					else
					{
						pElement->SetStateUnHover();//设置未框选元素样式
					}
				}
			}
			Cursor = IDC_ARROW;
			break;
		}
		}
	}
	else//没按住左键
	{
		TElement *pElementHover = nullptr;
		bool bInSel = false;


		for (auto pElement : pShape->Element)
			if (pElement->Picked(ptMouse, pConfig))
			{
				pElementHover = pElement;
				pElement->SetStateHover();//设置悬浮样式
				if (pElement->bDrawSquare)
					bInSel = true;//浮动到选择集上方
			}
			else
				pElement->SetStateUnHover();


		if (bInSel)
			for (auto pElement : PickedElements)
				pElement->SetStateHover();

		if (pElementHover)
		{
			if (eMode == SELECT_MOVE)
				sTips = TEXT("可拖动（无约束）");
			else
			{
				if (pElementHover->CanBeDragged())
					sTips = TEXT("可拖动（带约束）");
				else
					sTips = TEXT("不可拖动");
			}
		}
		else
			sTips.clear();

		Cursor = IDC_ARROW;
	}

	pCanvas->Invalidate();
}


void TSelectTool::OnLButtonUp(HWND hWnd, UINT nFlags, POINT ptPos)
{
	bShowSelRect = false;

	if (eState == NONE)
	{
		//清点框选内容，设置样式，进入选择集
		PickedElements.clear();
		for (auto pElement : pShape->Element)
		{
			if (pElement->InSelect(rcSelect, bSelCross, pConfig))
			{
				PickedElements.push_back(pElement);
				pElement->SetStateChosen();
			}
			else
			{
				pElement->SetStateNormal();
			}
		}

		//刷新悬浮
		for (auto pElement : pShape->Element)
			if (pElement->Picked(ptPos, pConfig))
				pElement->SetStateHover();
			else
				pElement->SetStateUnHover();
	}

	if (PickedElements.size() == 1)
	{
		PickedElements.front()->NoticeListView(pListView);
	}
	else
	{
		pListView->DeleteAllItems();
	}

	pCanvas->Invalidate();
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//CancelTreeViewAndListView();
	//RestorePickedLineStyle();

	//设置选区框起始点
	rcSelect.left = ptPos.x;
	rcSelect.top = ptPos.y;

	TElement *pElement = pShape->GetPickedElement(ptPos, pConfig);

	if (pElement != nullptr)
	{
		if (pElement->bDrawSquare)//点击了选择集中的元素
		{
			if (eMode == SELECT_MOVE)
			{
				eState = MOVE_SEL;
			}
			else
				eState = DRAG_SEL;
		}
		else
		{
			if (eMode == SELECT_MOVE)
			{
				eState = MOVE_SEL;
			}
			else
			{
				eState = DRAG_SEL;
				pSolver->RecordStartDragPos(pElement, pConfig->ScreenToReal(ptPos));
			}

			Select(pElement,true,true);
		}
		ptDrag = { rcSelect.left, rcSelect.top };
		//pCanvas->Invalidate();
		return;
	}


	eState = NONE;
}

void TSelectTool::Select(std::vector<TElement*> &vecSel)
{

}

void TSelectTool::Select(const TElement *pElement, bool ChooseTreeView, bool UpdateListView)
{
	for (auto pEle : PickedElements)
		pEle->SetStateNormal();

	PickedElements.clear();
	for (auto pEle : pShape->Element)
		if (pEle == pElement)
		{
			pEle->SetStateChosen();
			PickedElements.push_back(pEle);
			break;
		}

	if (pElement == nullptr)
	{
		CancelTreeViewAndListView();
	}
	else
	{
		if (UpdateListView)
			const_cast<TElement*>(pElement)->NoticeListView(pListView);

		if (ChooseTreeView)
			pTreeViewContent->SelectById(pElement->id);
	}

	pCanvas->Invalidate();
}

void TSelectTool::SelectAll()
{
	for (auto pEle : pShape->Element)
	{
		pEle->SetStateChosen();
		PickedElements.push_back(pEle);
	}
	pCanvas->Invalidate();
}

void TSelectTool::SelectById(int id, bool ChooseTreeView, bool UpdateListView)
{
	Select(pShape->GetElementById(id), ChooseTreeView, UpdateListView);
}

void TSelectTool::CancelTreeViewAndListView()
{
	//通知TreeView取消选中
	pTreeViewContent->SelectNull();

	//通知ListView清空
	pListView->DeleteAllItems();
}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	bShowSelRect = false;
	ptMouse = ptPos;

	TElement *pElement = pShape->GetPickedElement(ptPos, pConfig);

	if (pElement != nullptr)
	{
		if (!pElement->bDrawSquare)//不在选择集
			Select(pElement, true, true);

		pElement->NoticeListView(pListView);

		sTips.clear();
		pCanvas->Invalidate();

		//弹出右键菜单，
		HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_RIGHT));
		hMenu = GetSubMenu(hMenu, 0);
		ClientToScreen(hWnd, &ptPos);
		
		//此处句柄为Canvas句柄，消息上传至MainWindow，由MainWindow接收发送DELETE键至本级
		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, ptPos.x, ptPos.y, 0, hWnd, NULL);
		//}
	}
	else
	{
		CancelTreeViewAndListView();
		Select(nullptr,true,true);
	}
}

void TSelectTool::OnRButtonUp(HWND hWnd, UINT uMsg, POINT ptPos)
{

}

void TSelectTool::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (lParam == MAKELONG(MOD_CONTROL,'A'))
	{
		SelectAll();
		pCanvas->Invalidate();
	}
}

//由祖先插入WM_PAINT事件中进行绘制
void TSelectTool::Draw(HDC hdc)
{
	for (auto pElement : PickedElements)
		pElement->DrawPickSquare(hdc, pConfig);

	//画选择框
	if (bShowSelRect)
	{
		TDraw::DrawRect(hdc, rcSelect, pConfig->logpenFront);

		TDrawTranslucent Trans;
		Trans.Start(hdc, 200, rcSelect, true);
		Trans.Input(&rcSelect);
		//OutputDebugPrintf(TEXT("DRAW:x%d y%d x%d y%d\n\n"), rcSelect.left, rcSelect.top, rcSelect.right, rcSelect.bottom);
		TDraw::FillRect(hdc, &rcSelect, RGB(255, 255, 255));
		Trans.End();

	}

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}

//仅用于添加原动件时的判断
bool TSelectTool::CanBeDriver(TElement *&pElementFront)
{
	if (PickedElements.size() == 1 && PickedElements.front()->CanBeDragged())
	{
		pElementFront = PickedElements.front();
		return true;
	}
	return false;
}