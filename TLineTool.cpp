#pragma once
#include "TLineTool.h"

#include "TCanvas.h"
#include "resource.h"
#include "TTool.h"
#include "TLine.h"
#include "TTreeViewContent.h"

#include "TConfiguration.h"
#include "TDraw.h"
#include "TLineEdit.h"
#include "TAttach.h"
#include "TConstraintCoincide.h"
#include "TTransfer.h"
#include "TSolver.h"

TLineTool::TLineTool()
{
	Attach = new TAttach(pCanvas->m_hWnd, pShape, pConfig);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(PS_SOLID, 1, pConfig->crPen);

	Line1 = new TLine;
	Line2 = new TLine;
	LineDim = new TLine;
	Line1->SetStyle(PS_DOT, 1, pConfig->crPen);
	Line2->SetStyle(PS_DOT, 1, pConfig->crPen);
	LineDim->SetStyle(PS_DOT, 1, pConfig->crPen);

	LineEdit = new TLineEdit;

	CoincideBegin = NULL;

	myElementType = ELEMENT_REALLINE;

	iPrevLineId = -1;
}


TLineTool::~TLineTool()
{
	DestroyWindow(LineEdit->m_hWnd);
	delete Attach;

	delete LineEdit;
	delete MoveLine;

	delete Line1;
	delete Line2;
	delete LineDim;

	if (CoincideBegin != NULL)
		delete CoincideBegin;
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}

void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos, MoveLine->ptBegin);
	MoveLine->ptEnd = Attach->dptAttach;

	if (dptHit.size() > 0)
	{
		//计算暂存线长度并存入Edit
		MoveLine->CalcLength();
		LineEdit->SetText(TEXT("%.3f"), MoveLine->dLength);
		SetFocus(LineEdit->m_hWnd);
		LineEdit->SetSelectAll();

		//计算尺寸线各点坐标
		int dist = 40;//暂存线和尺寸线距离
		double theta = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);
		pt1 = pConfig->RealToScreen(MoveLine->ptBegin);
		pt2 = pConfig->RealToScreen(MoveLine->ptEnd);
		Line1->ptBegin = pt1;
		Line2->ptBegin = pt2;
		if (MoveLine->ptEnd.y - MoveLine->ptBegin.y >= 0)//位于1,2象限
		{
			TDraw::Move(&pt1, 1, theta + M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta + M_PI / 2, dist);
		}
		else
		{
			TDraw::Move(&pt1, 1, theta - M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta - M_PI / 2, dist);
		}
		Line1->ptEnd = pt1;
		Line2->ptEnd = pt2;
		LineDim->ptBegin = pt1; LineDim->ptEnd = pt2;
		//若尺寸线长度为0则不显示
		if (LineDim->ptBegin.x == LineDim->ptEnd.x && LineDim->ptBegin.y == LineDim->ptEnd.y)
			bShowDimLine = false;
		else
			bShowDimLine = true;

		//设置Edit位置并显示
		if (LineEdit->m_hWnd != NULL && bShowDimLine == true)
		{
			int width = 60, height = 20;
			POINT ptMiddle;
			ptMiddle.x = (LineDim->ptEnd.x + LineDim->ptBegin.x) / 2;
			ptMiddle.y = (LineDim->ptEnd.y + LineDim->ptBegin.y) / 2;

			LineEdit->SetPos(ptMiddle.x - width / 2, ptMiddle.y - height / 2, width, height);
			LineEdit->SetVisible(true);
		}
		else
			LineEdit->SetVisible(false);
	}

	//由Canvas刷新
}

//由祖先插入WM_PAINT事件中进行绘制
void TLineTool::Draw(HDC hdc)
{

	Attach->Draw(hdc);

	//画临时线及尺寸线
	if (dptHit.size() > 0)
	{
		TDraw::DrawRealLine(hdc, *MoveLine, pConfig);//点过一次后才开始画临时线

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}
}



void TLineTool::AddIntoShape(TRealLine &RealLine)
{
	pShape->AddRealLine(RealLine);
}

void TLineTool::AddIntoTreeViewContent(TElement *Element, int id)
{
	if (Element->eType == ELEMENT_REALLINE)
		Element->eType = myElementType;
	pTreeViewContent->AddItem(Element, pShape->iNextId);
}

//LineEdit出现时Canvas是接收不到KEYDOWN的，自然LineTool也接收不到。
//KEYDOWN消息由LineEdit截获发送给Canvas，再传递下来。
void TLineTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		OnRButtonDown(hWnd, 0, { 0, 0 });
		break;
	case VK_SPACE:
	case VK_RETURN:
		if (LineEdit->GetVisible())
		{
			//得到字符并转换
			double num;
			TCHAR buffer[64];
			LineEdit->GetText(buffer);
			num = TTransfer::TCHAR2double(buffer);

			if (abs(num) > 1e-6)
			{
				TRealLine RealLine;
				RealLine.ptBegin = dptHit[dptHit.size() - 1];

				//计算得到终点坐标
				switch (Attach->GetiIvoryLine())
				{
				case 0:
					double angle;
					angle = TDraw::GetAngleFromPointReal(RealLine.ptBegin, MoveLine->ptEnd);
					RealLine.SetPoint(RealLine.ptBegin, num, angle);
					break;
				case 1://→
					RealLine.ptEnd = { RealLine.ptBegin.x + num, RealLine.ptBegin.y };
					RealLine.dLength = num;
					RealLine.dAngle = 0;
					break;
				case 3://←
					RealLine.ptEnd = { RealLine.ptBegin.x - num, RealLine.ptBegin.y };
					RealLine.dLength = num;
					RealLine.dAngle = M_PI;
					break;
				case 2://↑
					RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y + num };
					RealLine.dLength = num;
					RealLine.dAngle = M_PI/2;
					break;
				case 4://↓
					RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y - num };
					RealLine.dLength = num;
					RealLine.dAngle = 1.5*M_PI;
					break;
				}
				RealLine.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);

				//入库
				iPrevLineId = pShape->iNextId;
				AddIntoTreeViewContent(&RealLine, pShape->iNextId);
				AddIntoShape(RealLine);

				if (CoincideBegin != NULL)
				{
					//上一个约束入库
					CoincideBegin->ElementId2 = iPrevLineId;
					AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
					pShape->AddCoincide(*CoincideBegin);
					pSolver->RefreshEquations(true);

					delete CoincideBegin;
					CoincideBegin = NULL;
				}

				//加入连接约束：ID上一个.end=ID这个.begin
				CoincideBegin = new TConstraintCoincide;
				CoincideBegin->eElementType1 = myElementType;
				CoincideBegin->ElementId1 = iPrevLineId;//上一条线id
				CoincideBegin->Element1PointIndex = 2;//ptEnd

				CoincideBegin->eElementType2 = myElementType;
				//CoincideBegin->ElementId2 = pShape->iNextId;//此处未入库，此为当前序号
				CoincideBegin->Element2PointIndex = 1;//ptBegin

				//计算得出的终点存入暂存点集
				dptHit.push_back(RealLine.ptEnd);

				//设置临时线
				POINT ptNew = pConfig->RealToScreen(RealLine.ptEnd);
				InitialLine(RealLine.ptEnd);
				Attach->InitialLine(ptNew);

				LineEdit->SetVisible(false);//避免在画完线后还显示Edit

				//移动到新坐标
				TDraw::ClientPosToScreen(hWnd, &ptNew);
				SetCursorPos(ptNew.x, ptNew.y);
			}
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptPos = pConfig->RealToScreen(MoveLine->ptEnd);

	//处理约束
	if (dptHit.size() == 0)//第一点捕捉到的约束
	{
		if (Attach->iAttachElementId != -1)
		{
			//ID被捕捉=ID新元素.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->ElementId1 = Attach->iAttachElementId;
			CoincideBegin->Element1PointIndex = Attach->iAttachElementPointIndex;

			CoincideBegin->eElementType2 = myElementType;
			//CoincideBegin->ElementId2 = pShape->iNextId;//此处未入库，此为下一个Element的id
			CoincideBegin->Element2PointIndex = 1;//ptBegin
		}
	}
	else//非第一点
	{
		if (ptPos.x == ptPrevPos.x && ptPos.y == ptPrevPos.y)
			return;

		//若超出1个点则入库
		TRealLine RealLine;
		RealLine.SetStyle(pConfig->iStyle, pConfig->iWidth, pConfig->crPen);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);

		iPrevLineId = pShape->iNextId;
		AddIntoTreeViewContent(&RealLine, pShape->iNextId);
		AddIntoShape(RealLine);

		if (CoincideBegin != NULL)
		{
			//上一个约束入库
			CoincideBegin->ElementId2 = iPrevLineId;
			AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			pShape->AddCoincide(*CoincideBegin);
			pSolver->RefreshEquations(true);
			//RefreshTreeViewContent();
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		if (Attach->iAttachElementId != -1)//若终点捕捉上了
		{
			//ID被捕捉=ID这个.end
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->ElementId1 = Attach->iAttachElementId;
			CoincideBegin->Element1PointIndex = Attach->iAttachElementPointIndex;

			CoincideBegin->eElementType2 = myElementType;
			CoincideBegin->ElementId2 = iPrevLineId;//此处已入库，已入库元素id为nextid-1
			CoincideBegin->Element2PointIndex = 2;//ptEnd

			//终点连接入库
			AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			pShape->AddCoincide(*CoincideBegin);
			pSolver->RefreshEquations(true);

			delete CoincideBegin;
			CoincideBegin = NULL;
		}


			//加入连接约束：ID上一个.end=ID这个.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->eElementType1 = myElementType;
			CoincideBegin->ElementId1 = iPrevLineId;//上一条线id
			CoincideBegin->Element1PointIndex = 2;//ptEnd

			CoincideBegin->eElementType2 = myElementType;
			//CoincideBegin->ElementId2 = pShape->iNextId;//此处未入库，此为当前序号
			CoincideBegin->Element2PointIndex = 1;//ptBegin
	}

	//当前点存入暂存点集
	dptHit.push_back(MoveLine->ptEnd);

	//设置临时线
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//创建LineEdit或隐藏
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(pCanvas->m_hWnd, IDR_LineEdit, pCanvas->m_hInst);
	}
	else
		LineEdit->SetVisible(false);//避免在画完线后还显示Edit

	ptPrevPos = ptPos;
	//交给画布刷新
}

void TLineTool::InitialLine(DPOINT dptPos)
{
	POINT ptPos = pConfig->RealToScreen(dptPos);
	MoveLine->ptBegin = dptPos;
	MoveLine->ptEnd = dptPos;

	Line1->ptBegin = ptPos;
	Line1->ptEnd = ptPos;
	Line2->ptBegin = ptPos;
	Line2->ptEnd = ptPos;
	LineDim->ptBegin = ptPos;
	LineDim->ptEnd = ptPos;
	bShowDimLine = true;
}

void TLineTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	if (dptHit.size() == 0)//没有点的情况下点右键则重置工具
		::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
	else
	{
		//若已创建Edit则隐藏Edit
		if (LineEdit->m_hWnd != NULL)
			LineEdit->SetVisible(false);

		bShowDimLine = false;

		//消除暂存重合约束
		if (CoincideBegin != NULL)
		{
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		dptHit.swap(std::vector<DPOINT>());

		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), false);

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}