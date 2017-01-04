#pragma once
#include "TSelectTool.h"

#include "TCanvas.h"
#include "TShape.h"
#include "TListView.h"
#include "TTreeViewContent.h"
#include "TConstraintCoincide.h"

#include "TDraw.h"
#include "TConfiguration.h"

#include "TSolver.h"

TSelectTool::TSelectTool()
{
	bDrag = false;
	iPickIndex = -1;
	iPrevPickIndex = -1;
	iHoverIndex = -1;
	Cursor = IDC_ARROW;
}

//由TTool的虚析构函数重载
TSelectTool::~TSelectTool()
{
	if (iPickIndex != -1)
	{
		//恢复线型
		RestorePickedLineStyle();
		RestoreHoveredLineStyle();

		CancelTreeViewAndListView();
	}
}

void TSelectTool::RestorePickedLineStyle()
{
	while (PickedLineId.size() > 0)
	{
		pShape->GetElementById(PickedLineId.top())->logpenStyleShow.lopnStyle = pShape->GetElementById(PickedLineId.top())->logpenStyle.lopnStyle;
		PickedLineId.pop();
	}
}

void TSelectTool::RestoreHoveredLineStyle()
{
	while (HoveredLineId.size() > 0)
	{
		pShape->GetElementById(HoveredLineId.top())->logpenStyleShow.lopnColor = pShape->GetElementById(HoveredLineId.top())->logpenStyle.lopnColor;
		HoveredLineId.pop();
	}
}

void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickIndex != -1)
		{
			//先恢复线型，否则删掉元素后index将失效
			RestorePickedLineStyle();
			RestoreHoveredLineStyle();

			int id = pShape->Element[iPickIndex]->id;
			std::vector<int> InfluenceId;// = pShape->GetInfluenceId(id);

			pTreeViewContent->DeleteById(pShape->Element[iPickIndex]->id);
			InfluenceId = pShape->DeleteElement(iPickIndex);

			for (int i = 0; i < InfluenceId.size(); i++)
				pTreeViewContent->DeleteById(InfluenceId[i]);

			iPickIndex = -1;
			iPrevPickIndex = -1;
			iHoverIndex = -1;

			CancelTreeViewAndListView();

			pSolver->RefreshEquations(true);
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
	//::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	if (bDrag)
	{
		pSolver->ClearOutput();
		pSolver->ClearConstraint();
		pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
		pSolver->Solve(true);
		return;
	}

	RestoreHoveredLineStyle();
	iHoverIndex = -1;

	//遍历所有
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
			if (PickRealLine(ptPos, pShape->Element[i]))//发现拾取
			{
				iHoverIndex = i;
			}
			break;
		case ELEMENT_FRAMEPOINT:
			if (TDraw::PointInFramePoint(pConfig->RealToScreen(((TFramePoint *)(pShape->Element[i]))->dpt), ptPos))
			{
				iHoverIndex = i;
			}
			break;
		}
		if (iHoverIndex != -1)
		{
			if (iPickIndex != iHoverIndex)//若浮过的线已被选中则不变色
			{
				//暂存当前线型并更改
				pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor = RGB(200, 200, 200);
				HoveredLineId.push(pShape->Element[iHoverIndex]->id);
			}
			return;
		}
	}
}

//应在调用前进行类型判断，避免将非TRealLine *型元素传入
bool TSelectTool::PickRealLine(POINT ptPos, TElement *Element)
{
	TRealLine *pRealLine = (TRealLine *)(Element);
	POINT pt1 = pConfig->RealToScreen(pRealLine->ptBegin);
	POINT pt2 = pConfig->RealToScreen(pRealLine->ptEnd);
	double length = TDraw::Distance(pt1, pt2);
	double length1 = TDraw::Distance(ptPos, pt1);
	double length2 = TDraw::Distance(ptPos, pt2);

	if (length1 + length2 - length <= 0.5)//容差
		return true;
	else
		return false;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	CancelTreeViewAndListView();
	RestorePickedLineStyle();
	iPickIndex = -1;

	//遍历所有
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
			if (PickRealLine(ptPos, pShape->Element[i]))//发现拾取
			{
				iPickIndex = i;
				if (bDrag)
				{
					iPrevPickIndex = -1;
					bDrag = false;
					Cursor = IDC_ARROW;
				}
				else
				{
					if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
					{
						bDrag = true;
						Cursor = IDC_HAND;
					}
					iPrevPickIndex = i;
				}
			}
			break;
		case ELEMENT_REALLINE:
			if (PickRealLine(ptPos, pShape->Element[i]))//发现拾取
			{
				iPickIndex = i;
			}
			break;
		case ELEMENT_FRAMEPOINT:
			if (TDraw::PointInFramePoint(pConfig->RealToScreen(((TFramePoint *)(pShape->Element[i]))->dpt), ptPos))
			{
				iPickIndex = i;
			}
			break;
		}
		if (iPickIndex != -1)
		{
			//暂存当前线型并更改
			pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;
			PickedLineId.push(pShape->Element[iPickIndex]->id);

			//通知TreeView选中
			pTreeViewContent->SelectById(pShape->Element[iPickIndex]->id);

			//通知ListView更新
			pShape->Element[iPickIndex]->NoticeListView(pListView);
			return;
		}
	}

}

void TSelectTool::SelectById(int id)
{
	RestorePickedLineStyle();
	iPickIndex = -1;

	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		if (pShape->Element[i]->id == id)
		{
			iPickIndex = i;
			//暂存当前线型并更改
			pShape->Element[iPickIndex]->logpenStyleShow.lopnStyle = PS_DOT;
			PickedLineId.push(pShape->Element[iPickIndex]->id);

			//通知ListView更新
			//pShape->Element[iPickIndex]->NoticeListView(pListView);

			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			return;
		}
	}
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
	if (bDrag)
	{
		iPrevPickIndex = -1;
		bDrag = false;
		Cursor = IDC_ARROW;
	}

	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_FRAMEPOINT:
			break;
		}
	}

}

//由祖先插入WM_PAINT事件中进行绘制
void TSelectTool::Draw(HDC hdc)
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptEnd));
			break;

		case ELEMENT_FRAMEPOINT:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->Element[iPickIndex])->dpt));
			break;
		case CONSTRAINT_COINCIDE:
			if (pShape->Element[iPickIndex]->available == true)
			{
				//画重合点
				int id = ((TConstraintCoincide *)pShape->Element[iPickIndex])->ElementId1;
				switch (((TConstraintCoincide *)pShape->Element[iPickIndex])->eElementType1)
				{
				case ELEMENT_BAR:
				case ELEMENT_REALLINE:
					if (((TConstraintCoincide *)pShape->Element[iPickIndex])->Element1PointIndex == 1)
						TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->GetElementById(id))->ptBegin));
					else
						TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->GetElementById(id))->ptEnd));
					break;
				case ELEMENT_FRAMEPOINT:
					TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->GetElementById(id))->dpt));
					break;
				}
			}
			break;
		}
		//由于线型变化，且画线位于工具绘制之前，所以需要刷新一次
		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);

	}
}