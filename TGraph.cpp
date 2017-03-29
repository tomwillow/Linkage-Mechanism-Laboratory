#pragma once
#include "TGraph.h"

#include "resource.h"
#include "TDraw.h"

#include "TConfiguration.h"

TGraph::TGraph(TConfiguration *pConfig)
{
	this->pConfig = pConfig;

	LineMouseX.SetStyle(PS_DOT, 1, 0);
	LineMouseY.SetStyle(PS_DOT, 1, 0);

	iPick = -1;

	iMargin = 0;
}


TGraph::~TGraph()
{
	DestroyWindow(m_hWnd);
	delete[] ptArray;
}

void TGraph::SetMargin(int iMargin)
{
	this->iMargin = iMargin;
}

void TGraph::OnSize(WPARAM wParam, LPARAM lParam)
{
	rcGraph = TDraw::GetMarginRect(ClientRect, iMargin);
	CalcPointArray();
	Invalidate();
}

void TGraph::InputDptVector(std::vector<DPOINT> &dptInputVector)
{
	dptVector = dptInputVector;
	if (dptVector.size() > 0)
	{
		x_max = x_min = dptVector[0].x;
		y_max = y_min = dptVector[0].y;
	}

	for (auto dpt : dptVector)
	{
		if (dpt.x > x_max) x_max = dpt.x;
		if (dpt.x < x_min) x_min = dpt.x;
		if (dpt.y > y_max) y_max = dpt.y;
		if (dpt.y < y_min) y_min = dpt.y;
	}
	x_len = x_max - x_min;
	y_len = y_max - y_min;

	iPtCount = dptVector.size();
	ptArray = new POINT[iPtCount];
	CalcPointArray();

	Invalidate();
}

void TGraph::CalcPointArray()
{
	for (int i = 0; i < dptVector.size();++i)
	{
		ptArray[i] = TDraw::DPOINT2POINT(dptVector[i], x_min, x_max, y_min, y_max, rcGraph);
	}
}

void TGraph::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//Ìî³ä±³¾°
	TDraw::FillRect(hdc, &ClientRect, RGB(255,255,255));

	TDraw::FillRect(hdc, &rcGraph, pConfig->crGraphBackground);

	TDraw::DrawGrid(hdc, rcGraph, pConfig,pConfig->crGraphGridBig,pConfig->crGraphGridSmall);

	LOGPEN logpen = { PS_SOLID, { 1, 0 }, 0 };
	TDraw::DrawPolyline(hdc, ptArray, iPtCount, logpen);

	if (PtInRect(&rcGraph, ptMouse))
	{
		TDraw::DrawLine(hdc, LineMouseX);
		TDraw::DrawLine(hdc, LineMouseY);
	}

	if (iPick != -1)
		TDraw::DrawPickSquare(hdc, ptArray[iPick]);
}

void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	LineMouseX.ptBegin = { rcGraph.left, ptMouse.y };
	LineMouseX.ptEnd = { rcGraph.right, ptMouse.y };

	LineMouseY.ptBegin = { ptMouse.x,rcGraph.top };
	LineMouseY.ptEnd = { ptMouse.x,rcGraph.bottom };

	DPOINT dptMouse;
	dptMouse = TDraw::POINT2DPOINT(ptMouse, x_min, x_max, y_min, y_max, rcGraph);

	SetText(TEXT("(%f,%f)"), dptMouse.x, dptMouse.y);

	OnLButtonDown(hWnd, uMsg, wParam, lParam);

	Invalidate();
}

void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	double dist, min_dist=10;
	iPick = -1;
	for (int i = 0; i < iPtCount; ++i)
	{
		dist = TDraw::Distance(ptArray[i], ptMouse);
		if (dist < min_dist)
		{
			min_dist = dist;
			iPick = i;
		}
	}

	if (iPick != -1)
	{
		SetText(TEXT("(%f,%f)"), dptVector[iPick].x,dptVector[iPick].y);
		Invalidate();
	}
}

bool TGraph::OnClose()
{
	//ShowWindow(SW_HIDE);
	PostMessage(m_hParent, WM_COMMAND, ID_DELETE_GRAPH, (LPARAM)this);
	return true;
}