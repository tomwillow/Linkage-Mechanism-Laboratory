#pragma once
#include "DetectMemoryLeak.h"

#include "TSelectTool.h"

#include "TSlider.h"
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
	eMode = SELECT_MOVE;
	//bShowTips = false;
	bDrag = false;
	bMove = false;
	iPickIndex = -1;
	iPrevPickIndex = -1;
	iHoverIndex = -1;
	Cursor = IDC_ARROW;

	bShowTips = true;
}

//由TTool的虚析构函数重载
TSelectTool::~TSelectTool()
{
	SelectNull();
}


void TSelectTool::SelectNull()
{

	if (iPickIndex != -1)
	{
		//恢复线型
		RestorePickedLineStyle();
		RestoreHoveredLineStyle();

		CancelTreeViewAndListView();
		iPickIndex = -1;
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

			//得到受影响的元素id，并删掉
			int id = pShape->Element[iPickIndex]->id;
			std::vector<int> InfluenceId;// = pShape->GetInfluenceId(id);

			pTreeViewContent->DeleteById(pShape->Element[iPickIndex]->id);
			InfluenceId = pShape->DeleteElement(iPickIndex);

			for (auto i : InfluenceId)
				pTreeViewContent->DeleteById(i);

			iPickIndex = -1;
			iHoverIndex = -1;
			EndDrag();
			EndMove();

			CancelTreeViewAndListView();

			pSolver->RefreshEquations(true);

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

	switch (eMode)
	{
	case SELECT_DRAG:
		if (bDrag)
		{
			pSolver->ClearOutput();
			pSolver->ClearConstraint();
			pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
			pSolver->Solve(true);

			pCanvas->Invalidate();

			sTips = TEXT("移动鼠标可预览拖动结果");

			return;
		}
		break;
	case SELECT_MOVE:
		if (bMove)
		{
			DPOINT dptDelta = pConfig->ScreenToReal(ptPos) - pConfig->ScreenToReal(ptMouseClick);

			pShape->ChangePos(iPickIndex, dptDelta);

			ptMouseClick = ptPos;

			sTips = TEXT("点击以确定新位置");
			return;
		}
		break;
	}

	RestoreHoveredLineStyle();
	iHoverIndex = -1;

	//遍历所有以显示浮动效果
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
			if (TDraw::PointInRealLine(ptPos, (TRealLine *)pShape->Element[i], pConfig))//发现拾取
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
		case CONSTRAINT_COINCIDE:
			if (PickConstraintCoincide(ptPos, pShape->Element[i]))
			{
				iHoverIndex = i;
			}
			break;
		case ELEMENT_SLIDER:
			if (TDraw::PointInSlider(ptPos, (TSlider *)pShape->Element[i], pConfig))
			{
				iHoverIndex = i;
			}
			break;
		default:
			assert(0);
			break;
		}
		if (iHoverIndex != -1)//悬停有东西
		{
			if (iPickIndex != iHoverIndex)//浮过的线未被选中
			{
				//暂存当前线型并更改
				pShape->Element[iHoverIndex]->logpenStyleShow.lopnColor = RGB(200, 200, 200);
				HoveredLineId.push(pShape->Element[iHoverIndex]->id);
				sTips = TEXT("点击可选中");
				return;
			}
			else
			{
				//若浮过的线已被选中则不变色
				switch (eMode)
				{
				case SELECT_MOVE:
				sTips = TEXT("再次点击可进行移动");
					return;
				case SELECT_DRAG:
					sTips = TEXT("再次点击可进行拖动");
					return;
				}
			}
		}
		else
			sTips = TEXT("");
	}
}

bool TSelectTool::PickConstraintCoincide(POINT ptPos, TElement *element)
{
	TConstraintCoincide *temp = (TConstraintCoincide *)element;

	//重合虚线显示，且虚线被拾取
	if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig) && TDraw::PointInRealLine(ptPos, *(temp->pDpt[0]), *(temp->pDpt[1]), pConfig))
	{
		return true;
	}
	else
		return false;
}

void TSelectTool::EndDrag()
{
	iPrevPickIndex = -1;
	bDrag = false;
	Cursor = IDC_ARROW;
}

void TSelectTool::EndMove()
{
	iPrevPickIndex = -1;
	bMove = false;
	Cursor = IDC_ARROW;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	CancelTreeViewAndListView();
	RestorePickedLineStyle();
	iPickIndex = -1;

	if (bDrag)
	{
		EndDrag();
	}

	if (bMove)
	{
		EndMove();
	}

	//遍历所有
	for (size_t i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
			if (TDraw::PointInRealLine(ptPos, (TRealLine *)pShape->Element[i], pConfig))//发现拾取
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
		case CONSTRAINT_COINCIDE:
		{
			if (PickConstraintCoincide(ptPos, pShape->Element[i]))
			{
				iPickIndex = i;
			}
			break;
		}
		case ELEMENT_SLIDER:
			if (TDraw::PointInSlider(ptPos, (TSlider *)pShape->Element[i], pConfig))
			{
				iPickIndex = i;
			}
			break;
		default:
			assert(0);
			break;
		}

		if (iPickIndex != -1)//拾取到了
		{

			switch (eMode)
			{
			case SELECT_MOVE:
				//非拾取状态再次点击同一对象进入拾取
				if (bMove == false)
				{
					if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
					{
						bMove = true;
						Cursor = IDC_CROSS;
						ptMouseClick.x = ptPos.x;
						ptMouseClick.y = ptPos.y;
					}
					iPrevPickIndex = i;
				}
				break;
			case SELECT_DRAG:
				if (bDrag == false)
				{
					if (iPrevPickIndex != -1 && iPrevPickIndex == iPickIndex)
					{
						//非拾取状态再次点击同一对象进入拾取
						bDrag = true;
						Cursor = IDC_HAND;
					}
					iPrevPickIndex = i;
				}
				break;
			}

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
	//遍历结束
	if (iPickIndex == -1)
		iPrevPickIndex = -1;

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
			pShape->Element[iPickIndex]->NoticeListView(pListView);

			pCanvas->Invalidate();
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
	EndDrag();
	EndMove();
	SelectNull();

	pCanvas->Invalidate();

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
		case ELEMENT_SLIDEWAY:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TRealLine *)pShape->Element[iPickIndex])->ptEnd));
			break;

		case ELEMENT_FRAMEPOINT:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(((TFramePoint *)pShape->Element[iPickIndex])->dpt));
			break;
		case CONSTRAINT_COINCIDE:
		{
			TConstraintCoincide *temp = ((TConstraintCoincide *)pShape->Element[iPickIndex]);
			if (TDraw::ShowConstraintCoincideDotLine(temp, pConfig))
			{
				//画拾取方格
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[0])));
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[1])));
			}
			else
			{
				//画重合点
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(*(temp->pDpt[0])));
			}
		}
		break;
		case ELEMENT_SLIDER:
		{
			TSlider *pSlider = (TSlider *)pShape->Element[iPickIndex];
			for (auto iter = pSlider->vecDpt.begin(); iter != pSlider->vecDpt.end(); ++iter)

				//画拾取方格
				TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle)));
		}
			break;
		default:
			assert(0);
			break;
		}

	}

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, sTips.c_str(), pConfig);
}

//仅用于添加原动件时的判断
bool TSelectTool::CanBeDriver()
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_SLIDER:
			return true;
		}
	}
	return false;
}