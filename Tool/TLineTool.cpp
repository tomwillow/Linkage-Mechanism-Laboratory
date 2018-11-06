#pragma once
#include "TLineTool.h"

#include "..\Control\TCanvas.h"
#include "..\resource.h"
#include "..\Common\TCHAR_Function.h"
#include "..\Control\TTreeViewContent.h"

#include "..\TConfiguration.h"
#include "..\Draw\TDraw.h"
#include "..\Control\TLineEdit.h"
#include "..\Tool\TAttach.h"
#include "..\Common\TTransfer.h"
#include "..\Solver\TSolver.h"

#include "..\Element\TLine.h"
#include "..\Element\TRealLine.h"
#include "..\Element\TConstraintCoincide.h"

TLineTool::TLineTool()
{
	Attach = new TAttach(pCanvas, pShape, pConfig);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(pConfig);

	Line1 = new TLine;
	Line2 = new TLine;
	LineDim = new TLine;
	Line1->SetStyle(pConfig->logpenAssist);
	Line2->SetStyle(pConfig->logpenAssist);
	LineDim->SetStyle(pConfig->logpenAssist);

	LineEdit = new TLineEdit;

	CoincideBegin = NULL;

	myElementType = ELEMENT_REALLINE;

	pPrevLine = NULL;

	bShowTips = true;
	sType = TEXT("线段");
	bCanBuildCoincide = false;
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
	//bShowDptText = false;
	Attach->AttachAll(ptPos, MoveLine->ptBegin);
	//MoveLine->ptEnd = Attach->dptAttach;
	MoveLine->SetPoint(MoveLine->ptBegin, Attach->dptAttach);

	if (dptHit.size() > 0)
	{
		//计算暂存线长度并存入Edit
		//MoveLine->CalcLength();
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

			LineEdit->SetPosition(ptMiddle.x - width / 2, ptMiddle.y - height / 2, width, height);
			LineEdit->SetVisible(true);
		}
		else
			LineEdit->SetVisible(false);
	}

	ptMouse = pConfig->RealToScreen(Attach->dptAttach);
	if (ptMouse.x == ptPrevPos.x && ptMouse.y == ptPrevPos.y)
		sTips = TEXT("长度为0:请移动光标或输入数据");
	else
	{
		sTips = TEXT("点击以建立");
		sTips << sType;
		if (bShowDimLine)
			sTips << TEXT("\r\n（输入格式: 长度 or 长度<角度 or x坐标,y坐标（相对当前点））");
		if (Attach->bAttachedEndpoint && bCanBuildCoincide)
			sTips << TEXT("\r\n已吸附端点:自动建立重合约束");
	}

	//由Canvas刷新
}

//由祖先插入WM_PAINT事件中进行绘制
void TLineTool::Draw(HDC hdc)
{

	Attach->Draw(hdc);//主叉由Attach提供

	//画临时线及尺寸线
	if (dptHit.size() > 0)
	{
		switch (myElementType)
		{
		case ELEMENT_REALLINE://点过一次后才开始画临时线
			TDraw::DrawRealLine(hdc, *MoveLine, pConfig);
			break;
		case ELEMENT_BAR:
			TDraw::DrawBar(hdc, (TBar *)MoveLine, pConfig);
			break;
		case ELEMENT_SLIDEWAY:
			TDraw::DrawSlideway(hdc, (TSlideway *)MoveLine, pConfig);
			break;
		}

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}

TElement * TLineTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(0));
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(1));

	RealLine.eType = myElementType;
	return pShape->AddElement(&RealLine);
}

void TLineTool::AddIntoTreeViewContent(TElement *Element, int id)
{
	//约束也要经过此处
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
			TRealLine RealLine;
			RealLine = *MoveLine;

			//入库
			AddIntoTreeViewContent(&RealLine, pShape->iNextId);
			pPrevLine = AddIntoShape(RealLine);

			if (CoincideBegin != NULL)
			{
				//上一个约束入库
				CoincideBegin->pElement[1] = pPrevLine;

				AddCoincide(CoincideBegin, pShape->iNextId, pConfig);

				pSolver->RefreshEquations();

				delete CoincideBegin;
				CoincideBegin = NULL;
			}

			//加入连接约束：ID上一个.end=ID这个.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);

			CoincideBegin->pElement[0] = pPrevLine;
			CoincideBegin->PointIndexOfElement[0] = 1;//ptEnd

			CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin

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
		break;
	default:
		//所有键经过此处，不止是Edit内的

		//得到字符并转换
		double length, DegAngle;
		DPOINT dpt;
		TCHAR buffer[64];
		LineEdit->GetText(buffer);

		std::vector<TCHAR *>szNums;
		TCHAR_Function::Split(buffer, szNums, TEXT(","));//切分坐标

		enum enumInputType{ INPUT_BAD, INPUT_LENGTH, INPUT_COORDINATE, INPUT_LENGTH_ANGLE } eInputType;
		if (szNums.size() == 2)//确实是坐标
		{
			eInputType = INPUT_COORDINATE;
			dpt.x = TTransfer::TCHAR2double(szNums[0]);
			dpt.y = TTransfer::TCHAR2double(szNums[1]);
		}
		else
		{
			szNums.clear();
			TCHAR_Function::Split(buffer, szNums, TEXT("<"));
			length = TTransfer::TCHAR2double(szNums[0]);
			if (abs(length) > 1e-6)//长度>0
				if (szNums.size() == 2)
				{
					eInputType = INPUT_LENGTH_ANGLE;
					DegAngle = TTransfer::TCHAR2double(szNums[1]);
				}
				else
				{
					eInputType = INPUT_LENGTH;
				}
			else
				eInputType = INPUT_BAD;
		}
		TCHAR_Function::ReleaseVectorTCHAR(szNums);
		//已得到dpt,length,angle

		switch (eInputType)
		{
		case INPUT_COORDINATE:
			MoveLine->ptEnd.x = MoveLine->ptBegin.x + dpt.x;
			MoveLine->ptEnd.y = MoveLine->ptBegin.y + dpt.y;
			break;
		case INPUT_LENGTH:
			DegAngle = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);//返回值为弧度
			MoveLine->SetPointByIvoryLine(Attach->GetiIvoryLine(), MoveLine->ptBegin, length, DegAngle);
			break;
		case INPUT_LENGTH_ANGLE:
			MoveLine->SetPointByDegAngle(MoveLine->ptBegin, length, DegAngle);
			break;
		}
		//bShowDptText = true;
		//dptText = MoveLine->ptEnd;

		Attach->dptAttach = MoveLine->ptEnd;

		pCanvas->Invalidate();

		break;
	}

}

void TLineTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)
{
	if (bCanBuildCoincide)
	{
		AddIntoTreeViewContent(pCoincide, id);
		pShape->AddElement(pCoincide);
	}
}


void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//bShowDptText = false;
	ptPos = pConfig->RealToScreen(MoveLine->ptEnd);

	//处理约束
	if (dptHit.size() == 0)//第一点捕捉到的约束
	{
		if (Attach->bAttachedEndpoint)
		{
			//ID被捕捉=ID新元素.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);
			CoincideBegin->pElement[0] = Attach->pAttachElement;
			CoincideBegin->PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

			CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin
		}
	}
	else//非第一点
	{
		if (ptPos.x == ptPrevPos.x && ptPos.y == ptPrevPos.y)
			return;

		//若超出1个点则入库
		TRealLine RealLine;
		RealLine.SetStyle(MoveLine->logpenStyle);//只有此处线入库，复制MoveLine颜色，MoveLine颜色取新
		MoveLine->SetStyle(pConfig);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);

		//iPrevLineId = pShape->iNextId;
		AddIntoTreeViewContent(&RealLine, pShape->iNextId);
		pPrevLine = AddIntoShape(RealLine);

		if (CoincideBegin != NULL)
		{
			//上一个约束入库
			CoincideBegin->pElement[1] = pPrevLine;

			AddCoincide(CoincideBegin, pShape->iNextId, pConfig);
			//AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			//pShape->AddCoincide(*CoincideBegin, pConfig);

			pSolver->RefreshEquations();
			//RefreshTreeViewContent();
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		if (Attach->bAttachedEndpoint)//若终点捕捉上了
		{
			//ID被捕捉=ID这个.end
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);
			//CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->pElement[0] = Attach->pAttachElement;
			CoincideBegin->PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

			//CoincideBegin->eElementType2 = myElementType;
			CoincideBegin->pElement[1] = pPrevLine;//此处已入库，已入库元素id为nextid-1
			CoincideBegin->PointIndexOfElement[1] = 1;//ptEnd

			//终点连接入库
			AddCoincide(CoincideBegin, pShape->iNextId, pConfig);
			//AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			//pShape->AddCoincide(*CoincideBegin, pConfig);

			pSolver->RefreshEquations();

			delete CoincideBegin;
			CoincideBegin = NULL;
		}


		//加入连接约束：ID上一个.end=ID这个.begin
		CoincideBegin = new TConstraintCoincide;
		CoincideBegin->SetStyle(pConfig->logpen);
		//CoincideBegin->eElementType1 = myElementType;
		CoincideBegin->pElement[0] = pPrevLine;//上一条线id
		CoincideBegin->PointIndexOfElement[0] = 1;//ptEnd

		//CoincideBegin->eElementType2 = myElementType;
		//CoincideBegin->ElementId2 = pShape->iNextId;//此处未入库，此为当前序号
		CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin
	}

	//当前点存入暂存点集
	dptHit.push_back(MoveLine->ptEnd);

	//设置临时线
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//创建LineEdit或隐藏
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(pCanvas->m_hWnd, IDR_LINEEDIT, pCanvas->m_hInst);
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
	//bShowDptText = false;
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

		dptHit.clear();

		pCanvas->Invalidate();

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}