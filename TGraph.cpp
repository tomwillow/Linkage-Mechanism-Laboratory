#pragma once
#include "DetectMemoryLeak.h"
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
	CalcPointArray();
	Invalidate();
}

void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible)
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

	vecPointData.push_back(PointData);

	OnSize(0, 0);

	//CalcPointArray();

	//Invalidate();
}

void TGraph::CalcPointArray()
{
	double x_max, x_min, y_max, y_min;
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

	for (auto &PointData : vecPointData)
	{
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

void TGraph::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//填充背景
	TDraw::FillRect(hdc, &ClientRect, pConfig->crGraphBackground);


	TDraw::FillRect(hdc, &(rcGraph), RGB(255, 255, 255));


	if (bShowMouseLine && PtInRect(&(rcGraph), ptMouse))
	{
		TDraw::DrawLine(hdc, LineMouseX);
		TDraw::DrawLine(hdc, LineMouseY);
	}

	//画线
	for (auto PointData : vecPointData)
	{
		if (PointData.Show == true)
			TDraw::DrawPolyline(hdc, PointData.ptArray, PointData.iPtCount, PointData.logpen);
	}

	if (bShowMouseLine && iPickPointDataIndex != -1 && vecPointData[iPickPointDataIndex].Show)
	{
		static TCHAR szCoordinate[32];
		_stprintf(szCoordinate, TEXT("(%f,%f)"), vecPointData[iPickPointDataIndex].dptVector[iPick].x, vecPointData[iPickPointDataIndex].dptVector[iPick].y);

		//显示坐标
		TDraw::DrawTips(hdc, ptMouse, szCoordinate, pConfig);

		//画方块
		TDraw::DrawPickSquare(hdc, vecPointData[iPickPointDataIndex].ptArray[iPick]);
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

		SetText(TEXT("(%f,%f)"), vecPointData[iPickPointDataIndex].dptVector[iPick].x, vecPointData[iPickPointDataIndex].dptVector[iPick].y);

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
	//ShowWindow(SW_HIDE);
	PostMessage(m_hParent, WM_COMMAND, ID_DELETE_GRAPH, (LPARAM)this);
	return true;
}