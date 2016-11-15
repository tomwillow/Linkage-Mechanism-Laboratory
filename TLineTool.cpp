#pragma once
#include "TLineTool.h"

#include "resource.h"
#include "TTool.h"
#include "TLine.h"

#include "TConfiguration.h"
#include "TMainWindow.h"
#include "TDraw.h"
#include "TLineEdit.h"
#include "TAttach.h"

extern TMainWindow win;
TLineTool::TLineTool() 
{
	Attach = new TAttach(win.Canvas.m_hWnd, &(win.m_Shape), &(win.m_Configuration));
	Config = &(win.m_Configuration);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(PS_SOLID, 1, Config->crPen);

	Line1 = new TLine;
	Line2 = new TLine; 
	LineDim = new TLine;
	Line1->SetStyle(PS_DOT, 1, Config->crPen);
	Line2->SetStyle(PS_DOT, 1, Config->crPen);
	LineDim->SetStyle(PS_DOT, 1, Config->crPen);

	LineEdit = new TLineEdit;
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
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}
 
void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	Attach->AttachAll(ptPos,MoveLine->ptBegin);
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
		pt1 = Config->RealToScreen(MoveLine->ptBegin);
		pt2 = Config->RealToScreen(MoveLine->ptEnd);
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
		TDraw::DrawRealLine(hdc, *MoveLine,Config);//点过一次后才开始画临时线

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}
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

			TRealLine RealLine;
			RealLine.ptBegin = dptHit[dptHit.size() - 1];

			//计算得到终点坐标
			switch (Attach->GetiIvoryLine())
			{
			case 0:
				double angle;
				angle=TDraw::GetAngleFromPointReal(RealLine.ptBegin, MoveLine->ptEnd);
				RealLine.SetPoint(RealLine.ptBegin, num, angle);
				break;
			case 1:
				RealLine.ptEnd = { RealLine.ptBegin.x + num, RealLine.ptBegin.y };
				RealLine.dLength = num;
				break;
			case 3:
				RealLine.ptEnd = { RealLine.ptBegin.x - num, RealLine.ptBegin.y };
				RealLine.dLength = num;
				break;
			case 2:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y + num };
				RealLine.dLength = num;
				break;
			case 4:
				RealLine.ptEnd = { RealLine.ptBegin.x, RealLine.ptBegin.y - num };
				RealLine.dLength = num;
				break;
			}
			RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);

			//入库
			win.m_Shape.AddRealLine(RealLine);

			//计算得出的终点存入暂存点集
			dptHit.push_back(RealLine.ptEnd);

			//设置临时线
			POINT ptNew = Config->RealToScreen(RealLine.ptEnd);
			InitialLine(RealLine.ptEnd);
			Attach->InitialLine(ptNew);

			LineEdit->SetVisible(false);//避免在画完线后还显示Edit

			//移动到新坐标
			TDraw::ClientPosToScreen(hWnd, &ptNew);
			SetCursorPos(ptNew.x, ptNew.y);
		}
		break;
	}
}
void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptPos = Config->RealToScreen(MoveLine->ptEnd);

	//上一点及当前点存入数据
	if (dptHit.size() > 0)
	{
		TRealLine RealLine;
		RealLine.SetStyle(Config->iStyle, Config->iWidth, Config->crPen);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);
		win.m_Shape.AddRealLine(RealLine);
	}

	//当前点存入暂存点集
	dptHit.push_back(MoveLine->ptEnd);

	//设置临时线
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//创建LineEdit或隐藏
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(win.Canvas.m_hWnd, IDR_LineEdit, win.Canvas.m_hInst);
		LineEdit->Load(hWnd);
	}
	else
		LineEdit->SetVisible(false);//避免在画完线后还显示Edit

	//交给画布刷新
}

void TLineTool::InitialLine(DPOINT dptPos)
{
	POINT ptPos = Config->RealToScreen(dptPos);
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
	//若已创建Edit则隐藏Edit
	if (LineEdit->m_hWnd != NULL)
		LineEdit->SetVisible(false);

	bShowDimLine = false;


	//没有画线的情况下点右键则重置工具
	if (dptHit.size()==0)
		::PostMessage(win.m_hWnd, WM_COMMAND, ID_SELECT, 0);
	else
	{
		dptHit.clear();

		::InvalidateRect(win.Canvas.m_hWnd, &win.Canvas.ClientRect, false);

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}