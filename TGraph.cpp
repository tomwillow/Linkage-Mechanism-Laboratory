#pragma once
#include "DetectMemoryLeak.h"

#include <algorithm>
#include <functional>

#include "MyMath.h"
#include "TTransfer.h"

#include "TGraph.h"

#include "resource.h"
#include "TDraw.h"

#include "TConfiguration.h"

TGraph::TGraph(TConfiguration *pConfig)
{
	this->pConfig = pConfig;

	LineMouseX.SetStyle(pConfig->logpenMouseLine);
	LineMouseY.SetStyle(pConfig->logpenMouseLine);

	iPick = -1;
	iPickPointDataIndex = -1;

	iMargin = 0;
}


TGraph::~TGraph()
{
	DestroyWindow(m_hWnd);
	Clear();
}

void TGraph::Clear()
{
	for (auto PointData : vecPointData)
		delete[] PointData.ptArray;

	vecPointData.clear();

	iPick = -1;
	iPickPointDataIndex = -1;
}

void TGraph::SetMargin(int iMargin)
{
	this->iMargin = iMargin;
	OnSize(0, 0);
}

void TGraph::OnSize(WPARAM wParam, LPARAM lParam)
{
	rcGraph = TDraw::GetMarginRect(ClientRect, iMargin);
	CalcPointArray(rcGraph);
	Invalidate();

}

void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[])
{
	TPointData PointData;
	PointData.dptVector = dptInputVector;
	if (PointData.dptVector.size() > 0)
	{
		PointData.x_max = PointData.x_min = PointData.dptVector[0].x;
		PointData.y_max = PointData.y_min = PointData.dptVector[0].y;
	}

	for (auto dpt : PointData.dptVector)
	{
		if (dpt.x > PointData.x_max) PointData.x_max = dpt.x;
		if (dpt.x < PointData.x_min) PointData.x_min = dpt.x;
		if (dpt.y > PointData.y_max) PointData.y_max = dpt.y;
		if (dpt.y < PointData.y_min) PointData.y_min = dpt.y;
	}
	PointData.x_len = PointData.x_max - PointData.x_min;
	PointData.y_len = PointData.y_max - PointData.y_min;

	PointData.iPtCount = PointData.dptVector.size();
	PointData.ptArray = new POINT[PointData.iPtCount];

	PointData.logpen = logpen;

	PointData.Show = visible;

	PointData.sLegend = szLegend;

	//设置Legend大小
	RECT rcTemp;
	ZeroMemory(&rcTemp, sizeof(rcTemp));
	TDraw::DrawSystemFontText(GetDC(m_hWnd), PointData.sLegend.c_str(), rcTemp, 0, DT_CALCRECT);
	PointData.lLegendWidth = rcTemp.right - rcTemp.left;
	PointData.lLegendHeight = rcTemp.bottom - rcTemp.top;

	vecPointData.push_back(PointData);

	OnSize(0, 0);

	//CalcPointArray();

	//Invalidate();
}


void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible,const TCHAR szLegend[])
{
	if (vecX.size() != vecY.size()) return;

	std::vector<DPOINT> vecdpt(vecX.size());
	for (size_t i = 0; i < vecX.size(); ++i)
	{
		vecdpt[i].x = vecX[i];
		vecdpt[i].y = vecY[i];
	}
	InputDptVector(vecdpt, logpen, visible,szLegend);
}

void TGraph::CalcPointArray(const RECT &rcGraph)
{
	if (vecPointData.size() > 0)
	{
		x_max = vecPointData.front().x_max;
		x_min = vecPointData.front().x_min;
		y_max = vecPointData.front().y_max;
		y_min = vecPointData.front().y_min;
	}
	for (auto &PointData : vecPointData)
	{
		if (PointData.x_max > x_max) x_max = PointData.x_max;
		if (PointData.x_min < x_min) x_min = PointData.x_min;
		if (PointData.y_max > y_max) y_max = PointData.y_max;
		if (PointData.y_min < y_min) y_min = PointData.y_min;
	}

	//double x_len = x_max - x_min;
	//double y_len = y_max - y_min;
	//double delta_y = 0.1*y_len;
	//y_max += delta_y;
	//y_min -= delta_y;


	for (auto &PointData : vecPointData)
	{
		//PointData.y_max += delta_y;
		//PointData.y_min -= delta_y;
		//PointData.y_len += 2 * delta_y;
		for (size_t i = 0; i < PointData.dptVector.size(); ++i)
		{
			PointData.ptArray[i] = TDraw::DPOINT2POINT(PointData.dptVector[i], x_min, x_max, y_min, y_max, rcGraph);
		}
	}
}

void TGraph::SetPointDataVisible(int index, bool visible)
{
	if (index < vecPointData.size())
		vecPointData[index].Show = visible;
}

void TGraph::DrawGridAndScale(HDC hdc)
{

	int width = rcGraph.right - rcGraph.left;//显示区宽度
	int height = rcGraph.bottom - rcGraph.top;//显示区高度
	LONG lScaleXHeight = 24;//刻度+刻度文字 总高度
	LONG lScaleYWidth = 16;

	double x_len = x_max - x_min;
	double y_len = y_max - y_min;

	double x_all_scale_len(0.0);//刻度区实际范围
	int x_all_scale_number(0);//小刻度数量
	int x_all_scale_count(0);//大刻度数量

	int x_precision_digit(0);//有效数字结束处位于原数字的第几位 在小数点右边则为正

	SignificantDigit(x_len, 2, x_all_scale_number, x_all_scale_len, x_precision_digit);

	int min_px_per_grid = 10;
	int x_max_grid = width / min_px_per_grid;

	double num = pow(10, x_precision_digit + (x_min < 0 ? -1 : -1));
	double x_start = trunc(x_min*num) / num;//x刻度开始位置

	int units_per_big_grid;
	if (x_all_scale_number == 10)
	{
		units_per_big_grid = 1;
	}
	else
		if (x_all_scale_number <= 20)
			units_per_big_grid = 5;
		else
			if (x_all_scale_number > 80)
				units_per_big_grid = 10;
			else
				units_per_big_grid = 5;

	x_all_scale_count = x_all_scale_number / units_per_big_grid;//每5个单位一个大刻度

	double x_step = x_all_scale_len / x_all_scale_number * units_per_big_grid;// 

	DPOINT dpt;
	POINT ptBegin, ptEnd;
	POINT ptGridBigTop;
	LONG lScaleLong = 6;

	vecGridLineAndScaleX.clear();
	TGridLineAndScale GridLineAndScaleX;
	for (int i = -1; i <= x_all_scale_count + 2; ++i)
	{
		dpt.x = x_start + x_step*i;
		ptEnd = ptBegin = TDraw::DPOINT2POINT(dpt, x_min, x_max, y_min, y_max, rcGraph);
		ptBegin.y = rcGraph.bottom;
		ptEnd.y = ptBegin.y + lScaleLong;
		TDraw::DrawLine(hdc, ptBegin, ptEnd, pConfig->logpenBlack);//画刻度线

		//画刻度文字
		TCHAR szScaleX[64];
		TTransfer::double2TCHAR(dpt.x, szScaleX, x_precision_digit);
		RECT rcScaleX = { 0, 0, 0, 0 };
		TDraw::DrawSystemFontText(hdc, szScaleX, rcScaleX, 0, DT_CALCRECT);

		rcScaleX.left = ptEnd.x - rcScaleX.right / 2;
		rcScaleX.right = ptEnd.x + rcScaleX.right / 2;
		rcScaleX.top = ptEnd.y + 2;
		rcScaleX.bottom = rcScaleX.top + lTextHeight;
		TDraw::DrawSystemFontText(hdc, szScaleX, rcScaleX, 0, DT_CENTER);

		//画大网格线
		ptGridBigTop.x = ptBegin.x;
		ptGridBigTop.y = rcGraph.top;
		TDraw::DrawLine(hdc, ptGridBigTop, ptBegin, pConfig->logpenGraphGridBig);

		//画小网格线
		double x_step_small = x_step / units_per_big_grid;
		DPOINT dptSmall;
		POINT ptGridSmallTop, ptGridSmallBottom;
		for (int j = 1; j < units_per_big_grid; ++j)
		{
			dptSmall.x = dpt.x + x_step_small*j;
			ptGridSmallTop = ptGridSmallBottom = TDraw::DPOINT2POINT(dptSmall, x_min, x_max, y_min, y_max, rcGraph);
			ptGridSmallTop.y = rcGraph.top;
			ptGridSmallBottom.y = rcGraph.bottom;
			TDraw::DrawLine(hdc, ptGridSmallTop, ptGridSmallBottom, pConfig->logpenGraphGridSmall);
		}
	}
}

void TGraph::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//填充大背景
	TDraw::FillRect(hdc, &ClientRect, pConfig->crGraphBackground);

	//填充图背景-白色
	TDraw::FillRect(hdc, &(rcGraph), RGB(255, 255, 255));


	DPOINT dptOrg{ 0, 0 };
	POINT ptOrg = TDraw::DPOINT2POINT(dptOrg, x_min, x_max, y_min, y_max, rcGraph);

	//画网格
	//TDraw::DrawGrid(hdc, (rcGraph),ptOrg, pConfig->crGraphGridBig, pConfig->crGraphGridSmall, pConfig);


	//标题
	LONG lTextHeight(12);

	RECT rcTitle;
	rcTitle.left = rcGraph.left;
	rcTitle.right = rcGraph.right;
	rcTitle.bottom = rcGraph.top - 4;
	rcTitle.top = rcTitle.bottom - lTextHeight;
	TDraw::DrawSystemFontText(hdc, TEXT("Measure Result"), rcTitle, 0, DT_CENTER | DT_BOTTOM);

	//scale
	DrawGridAndScale(hdc);

	//x坐标单位
	RECT rcUnitsX;
	rcUnitsX.left = rcGraph.left;
	rcUnitsX.right = rcGraph.right;
	rcUnitsX.top = rcGraph.bottom + lScaleXHeight;
	rcUnitsX.bottom = rcUnitsX.top + lTextHeight;
	TDraw::DrawSystemFontText(hdc, TEXT("Time(sec)"), rcUnitsX, 0, DT_CENTER | DT_TOP);

	//y坐标单位
	RECT rcUnitsY = { 0, 0, 0, 0 };
	TDraw::DrawSystemFontText(hdc, TEXT("Position(mm)"), rcUnitsY, 0, DT_CALCRECT);
	rcUnitsY.top = rcGraph.top ;
	rcUnitsY.bottom = rcGraph.bottom +rcUnitsY.right;
	rcUnitsY.right = rcGraph.left - lScaleYWidth;
	rcUnitsY.left = rcUnitsY.right - lTextHeight;
	TDraw::DrawSystemFontTextVertical(hdc, TEXT("Position(mm)"), rcUnitsY, 0, DT_VCENTER|DT_SINGLELINE);

	//画鼠标线
	if (bShowMouseLine && PtInRect(&(rcGraph), ptMouse))
	{
		TDraw::DrawLine(hdc, LineMouseX);
		TDraw::DrawLine(hdc, LineMouseY);
	}

	//画数据线
	for (auto PointData : vecPointData)
	{
		if (PointData.Show == true)
			TDraw::DrawPolyline(hdc, PointData.ptArray, PointData.iPtCount, PointData.logpen);
	}

	//显示捕捉数据
	if (bShowMouseLine && iPickPointDataIndex != -1 && vecPointData[iPickPointDataIndex].Show)
	{
		static TCHAR szCoordinate[32];
		_stprintf(szCoordinate, TEXT("(%f,%f)"), vecPointData[iPickPointDataIndex].dptVector[iPick].x, vecPointData[iPickPointDataIndex].dptVector[iPick].y);

		//显示坐标
		TDraw::DrawTips(hdc, ptMouse, szCoordinate, pConfig);

		//画方块
		TDraw::DrawPickSquare(hdc, vecPointData[iPickPointDataIndex].ptArray[iPick]);
	}

	//画图例
	DrawLegend(hdc, pConfig->logpenBlack);

	//画黑框
	TDraw::DrawRect(hdc, TDraw::GetMarginRect(rcGraph, -1), pConfig->logpenBlack);

}

void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder)
{

	POINT ptUpperRight = { rcGraph.right - 10, rcGraph.top + 10 };
	LONG lTextHeight(0);
	LONG lAllTextWidth(0), lAllTextHeight(0);
	LONG lLineLong = 12;
	LONG lDistFromLineAndText = 4;
	for (auto &PointData : vecPointData)
	{
		if (PointData.Show)
		{
			if (PointData.lLegendWidth > lAllTextWidth)
				lAllTextWidth = PointData.lLegendWidth;
			lAllTextHeight += PointData.lLegendHeight;
			lTextHeight = PointData.lLegendHeight;
		}
	}
	LONG lInnerWidth =lLineLong+lDistFromLineAndText+ lAllTextWidth ;
	LONG lInnerHeight(lAllTextHeight);
	LONG lDelta(6);

	RECT rcLegend;
	rcLegend.right = ptUpperRight.x;
	rcLegend.top = ptUpperRight.y;
	rcLegend.left = rcLegend.right - lInnerWidth - 2 * lDelta;
	rcLegend.bottom = rcLegend.top + lInnerHeight + 2 * lDelta;
	TDraw::DrawRect(hdc, rcLegend, logpenBorder, RGB(255, 255, 255));

	RECT rcInner = TDraw::GetMarginRect(rcLegend, lDelta);

	LONG lLineStartY = rcInner.top + lTextHeight / 2;
	RECT rcText;
	rcText.left = rcInner.left + lLineLong+lDistFromLineAndText;
	rcText.right = rcText.left + lAllTextWidth;
	rcText.top = rcInner.top;
	rcText.bottom = rcText.top + lTextHeight;
	for (auto &PointData : vecPointData)
		if (PointData.Show)
	{
		TDraw::DrawLine(hdc, { rcInner.left, lLineStartY }, { rcInner.left + lLineLong, lLineStartY }, PointData.logpen);
		lLineStartY += lTextHeight;

		TDraw::DrawSystemFontText(hdc, PointData.sLegend.c_str(), rcText, PointData.logpen.lopnColor, DT_LEFT | DT_TOP);
		rcText.top += lTextHeight;
		rcText.bottom += lTextHeight;
	}
}

void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	AttachPoint();

	LineMouseX.ptBegin = { rcGraph.left, ptMouse.y };
	LineMouseX.ptEnd = { rcGraph.right, ptMouse.y };

	LineMouseY.ptBegin = { ptMouse.x, rcGraph.top };
	LineMouseY.ptEnd = { ptMouse.x, rcGraph.bottom };

	//OutputDebugPrintf(TEXT("%d,%d\n"), ptMouse.x, ptMouse.y);

	if (iPickPointDataIndex != -1)
	{
		dptMouse = TDraw::POINT2DPOINT(ptMouse, vecPointData[iPickPointDataIndex].x_min, vecPointData[iPickPointDataIndex].x_max, vecPointData[iPickPointDataIndex].y_min, vecPointData[iPickPointDataIndex].y_max, rcGraph);

		DPOINT dptAttached = vecPointData[iPickPointDataIndex].dptVector[iPick];
		//POINT ptAttached = TDraw::DPOINT2POINT(dptAttached,vecPointData[iPickPointDataIndex].x_min, vecPointData[iPickPointDataIndex].x_max, vecPointData[iPickPointDataIndex].y_min, vecPointData[iPickPointDataIndex].y_max, rcGraph);

		SetText(TEXT("(%f,%f)"), dptAttached.x, dptAttached.y);

		//LineMouseX.ptBegin.y=LineMouseX.ptEnd.y = ptAttached.y;
		//LineMouseY.ptBegin.x = LineMouseY.ptEnd.x = ptAttached.x;

		Invalidate();
	}
	else
		if (bShowMouseLine)
			Invalidate();
}

//输入 vecPointData, ptMouse
//输出 iPick, iPickPointDataIndex
void TGraph::AttachPoint()
{
	double dist, min_dist = 10;//px
	iPick = -1;
	iPickPointDataIndex = -1;
	for (size_t index = 0; index < vecPointData.size(); ++index)
		if (vecPointData[index].Show)
			for (int i = 0; i < vecPointData[index].iPtCount; ++i)
			{
				dist = TDraw::Distance(vecPointData[index].ptArray[i], ptMouse);
				if (dist < min_dist)//遍历所有 得到最近
				{
					min_dist = dist;
					iPick = i;
					iPickPointDataIndex = index;
				}
			}
}

void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AttachPoint();

	if (iPickPointDataIndex != -1)
	{
		SetText(TEXT("(%f,%f)"), vecPointData[iPickPointDataIndex].dptVector[iPick].x, vecPointData[iPickPointDataIndex].dptVector[iPick].y);
		Invalidate();
	}
	else
		if (bShowMouseLine)
			Invalidate();
}

bool TGraph::OnClose()
{
	if (bRealClose)
	{
		PostMessage(m_hParent, WM_COMMAND, ID_DELETE_GRAPH, (LPARAM)this);
	return true;
	}
	else
	{
	ShowWindow(SW_HIDE);
	return false;
	}
}