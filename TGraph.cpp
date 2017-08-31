#pragma once
#include "DetectMemoryLeak.h"

#include <algorithm>
#include <functional>
#include <numeric>

#include "MyMath.h"
#include "TTransfer.h"

#include "TGraph.h"

#include "resource.h"
#include "TDraw.h"

#include "TConfiguration.h"
#include "FileFunction.h"

using namespace std;

TGraph::TGraph(TConfiguration *pConfig) :iMarginTop(0), iMarginBottom(0), iMarginLeft(0), iMarginRight(0)
{
	this->pConfig = pConfig;

	LineMouseX.SetStyle(pConfig->logpenMouseLine);
	LineMouseY.SetStyle(pConfig->logpenMouseLine);

	iPick = -1;
	iPickPointDataIndex = -1;

}


TGraph::~TGraph()
{
	DestroyWindow(m_hWnd);
	Clear();
}

void TGraph::Clear()
{
	//for (size_t i = 0; i < vecPointData.size();++i)
	//	DeleteMenu(GetMenu(m_hWnd), ID_MENU_GRAPH_DATA_START + i, MF_BYCOMMAND);

	DeleteMenu(GetMenu(m_hWnd), (UINT)hMenuData, 0);

	for (auto PointData : vecPointData)
		delete[] PointData.ptArray;

	vecPointData.clear();

	//vecGridLineAndScale.clear();
	//vecGridScaleSmall.clear();

	iPick = -1;
	iPickPointDataIndex = -1;

}

void TGraph::SetMargin(int iMargin)
{
	iMarginTop = iMarginBottom = iMarginLeft = iMarginRight = iMargin;

	OnSize(0, 0);
}

void TGraph::Refresh()//刷新 等同于调用OnSize
{
	OnSize(0, 0);
}

void TGraph::OnSize(WPARAM wParam, LPARAM lParam)
{
	if (!bDraw)
		return;

	LONG lScaleLong = 6;//刻度长度
	LONG lScaleRightWidth = 0;

	rcGraph = ClientRect;
	//if (GetMenu(m_hWnd))
	//	rcGraph.bottom-=GetSystemMetrics(SM_CYMENUSIZE);

	//预计算间距
	{
		HDC hdc = GetDC(m_hWnd);
		lTextHeight = TDraw::GetSystemFontSize(hdc, TEXT("test")).y;

		LONG lAllScaleLeftWidth = 0;
		for (auto &PointData : vecPointData)
			if (PointData.Show)
			{
				CalcGridAndScale(hdc, PointData, lScaleBottomHeight, lScaleRightWidth,
					rcGraph, lAllScaleLeftWidth, lScaleLong, lInterval);
			}
		ReleaseDC(m_hWnd, hdc);

		//for_each(vecPointData.cbegin(), vecPointData.cend(), [&lAllScaleLeftWidth](const TPointData &PointData){lAllScaleLeftWidth += PointData.lScaleLeftWidth; });

		iMarginLeft = lInterval + lAllScaleLeftWidth;
		iMarginBottom = lScaleBottomHeight + (bShowLabelX ? (lInterval + lTextHeight) : 0);
		iMarginTop = bShowTitle ? (lTextHeight + lInterval) : 0;
		iMarginRight = lScaleRightWidth;
	}

	//间距生效
	rcGraph.top += iMarginTop;
	rcGraph.bottom -= iMarginBottom;
	rcGraph.left += iMarginLeft;
	rcGraph.right -= iMarginRight;

	TDraw::SetMarginRect(&rcGraph, 10);

	CalcPointArray(rcGraph);//计算显示点

	{
		HDC hdc = GetDC(m_hWnd);

		LONG lAllScaleLeftWidth = 0;
		for (auto &PointData : vecPointData)
			if (PointData.Show)
			{
				CalcGridAndScale(hdc, PointData, lScaleBottomHeight, lScaleRightWidth,
					rcGraph, lAllScaleLeftWidth, lScaleLong, lInterval);
			}

		ReleaseDC(m_hWnd, hdc);
	}

	Invalidate();
}

void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[], const TCHAR szUnitLabel[])
{
	TPointData PointData;
	PointData.dptVector = dptInputVector;

	//最大最小得到初值
	if (PointData.dptVector.size() > 0)
	{
		PointData.x_max = PointData.x_min = PointData.dptVector[0].x;
		PointData.y_max = PointData.y_min = PointData.dptVector[0].y;
	}

	//得到数据点边界
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
	PointData.ptArray = new POINT[PointData.iPtCount];//此时仅初始化，没有计算

	PointData.logpen = logpen;

	PointData.Show = visible;

	PointData.sLegend = szLegend;

	PointData.sLabelY = szUnitLabel;

	//设置Legend大小
	RECT rcTemp;
	ZeroMemory(&rcTemp, sizeof(rcTemp));
	HDC hdc = GetDC(m_hWnd);
	TDraw::DrawSystemFontText(hdc, PointData.sLegend.c_str(), rcTemp, 0, DT_CALCRECT);
	ReleaseDC(m_hWnd, hdc);
	PointData.lLegendWidth = rcTemp.right - rcTemp.left;
	PointData.lLegendHeight = rcTemp.bottom - rcTemp.top;

	//添加Menu
	HMENU hMenu = GetMenu(m_hWnd);//取得菜单句柄
	HMENU hMenuView = GetSubMenu(hMenu, 1);//“数据”菜单
	if (vecPointData.empty())//第一条数据则新建 数据 菜单句柄
	{
		hMenuData = CreateMenu();
		InsertMenu(hMenuView, 1, MF_POPUP | MF_BYPOSITION, (UINT)hMenuData, TEXT("数据"));

		PointData.bShowGridBig=PointData.bShowGridSmall = true;
	}
	AppendMenu(hMenuData, MF_CHECKED, ID_MENU_GRAPH_DATA_START + vecPointData.size(), PointData.sLegend.c_str());

	//入库
	vecPointData.push_back(PointData);

	OnSize(0, 0);
	//添加点必定经过此处
}


void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[], const TCHAR szUnitLabel[])
{
	if (vecX.size() != vecY.size()) return;

	std::vector<DPOINT> vecdpt(vecX.size());
	for (size_t i = 0; i < vecX.size(); ++i)
	{
		vecdpt[i].x = vecX[i];
		vecdpt[i].y = vecY[i];
	}
	InputDptVector(vecdpt, logpen, visible, szLegend, szUnitLabel);
}

void TGraph::CalcPointArray(const RECT &rcGraph)
{
	for (auto &PointData : vecPointData)
	{
		for (size_t i = 0; i < PointData.dptVector.size(); ++i)
		{
			PointData.ptArray[i] = TDraw::DPOINT2POINT(PointData.dptVector[i], PointData.x_min, PointData.x_max, PointData.y_min, PointData.y_max, rcGraph);
		}
	}
}

void TGraph::SetPointDataVisible(int index, bool visible)
{
	if (index >= 0 && (size_t)index < vecPointData.size())
		vecPointData[index].Show = visible;
}

void TGraph::DrawGridAndScale(HDC hdc)
{
	for (auto &PointData : vecPointData)
		if (PointData.Show)
		{
			for (auto &GridLineAndScale : PointData.vecGridLineAndScale)
			{
				//画刻度线	
				TDraw::DrawLine(hdc, GridLineAndScale.ptScaleBegin, GridLineAndScale.ptScaleEnd, pConfig->logpenBlack);

				//画刻度字
				TDraw::DrawSystemFontText(hdc, GridLineAndScale.sScale.c_str(), GridLineAndScale.rcScaleText, 0, DT_CENTER);

				//画大网格
				if (PointData.bShowGridBig)
					TDraw::DrawLine(hdc, GridLineAndScale.ptGridStart, GridLineAndScale.ptGridEnd, pConfig->logpenGraphGridBig);
			}
			TDraw::DrawLine(hdc, PointData.ptAxisBegin,PointData.ptAxisEnd, pConfig->logpenBlack);
		}

	//画小网格
	for (auto &PointData : vecPointData)
		if (PointData.Show)
			if (PointData.bShowGridSmall)
				for (auto &GridScaleSmall : PointData.vecGridScaleSmall)
				{
					TDraw::DrawLine(hdc, GridScaleSmall.ptBegin, GridScaleSmall.ptEnd, pConfig->logpenGraphGridSmall);
				}
}

void CalcGridParameters(HDC hdc, bool isX, const RECT &rcGraph, double v_min, double v_max,
	int &x_all_scale_count, int &x_precision_digit, double &x_start, double &x_step, double &x_step_small, int &small_grid_per_big_grid, bool &bDrawSmallGrid)
{

	int width = rcGraph.right - rcGraph.left;//显示区宽度
	int height = rcGraph.bottom - rcGraph.top;//显示区高度

	double x_len = v_max - v_min;

	double x_all_scale_len(0.0);//刻度区实际范围
	int x_all_scale_number(0);//小刻度数量

	SignificantDigit(x_len, 2, x_all_scale_number, x_all_scale_len, x_precision_digit);

	double num = pow(10, x_precision_digit);//+ (v_min < 0 ? -1 : -1)
	x_start = trunc(v_min*num) / num;//x刻度开始位置

	//试算刻度文字宽度
	LONG min_px_between_2_scale;//两个大网格间的允许最小像素，低于此值则大网格间隔加倍
	{
		RECT rcTemp;
		TCHAR szScaleX[64];
		TTransfer::double2TCHAR(x_start, szScaleX, x_precision_digit);
		rcTemp = { 0, 0, 0, 0 };
		TDraw::DrawSystemFontText(hdc, szScaleX, rcTemp, 0, DT_CALCRECT);
		if (isX)
			min_px_between_2_scale = rcTemp.right;
		else
			min_px_between_2_scale = rcTemp.bottom;
	}

	int units_per_big_grid = 1;
	x_all_scale_count = x_all_scale_number / units_per_big_grid;//每5个单位一个大刻度
	x_step = x_all_scale_len / x_all_scale_number * units_per_big_grid;//每个大刻度间实际距离

	LONG px_between_2_scale;
	while (1)
	{
		px_between_2_scale = isX ? TDraw::DPOINT2POINTXLEN(x_start, x_start + x_step, v_min, v_max, rcGraph) : TDraw::DPOINT2POINTYLEN(x_start, x_start + x_step, v_min, v_max, rcGraph);
		if (px_between_2_scale <= 0)
		{
			x_all_scale_count = 0;
			break;
		}
		if (px_between_2_scale >= min_px_between_2_scale + 4)
			break;

		switch (units_per_big_grid)
		{
		case 1:units_per_big_grid = 2; break;
		case 2:units_per_big_grid = 5; break;
		default:
			units_per_big_grid *= 2;
		}
		x_all_scale_count = x_all_scale_number / units_per_big_grid;//每5个单位一个大刻度
		x_step = x_all_scale_len / x_all_scale_number * units_per_big_grid;//每个大刻度间实际距离
	}

	//将x起点起算至刚好位于整格的前一格
	//before: x_min=136.4 x_strat=100 x_step=5
	//after: x_start=135
	if (x_start < v_min)
	{
		while (x_start < v_min)
		{
			x_start += x_step;
		}
		x_start -= x_step;
		++x_all_scale_count;
	}
	else
	{
		while (x_start > v_min)
		{
			x_start -= x_step;
			++x_all_scale_count;
		}
	}

	while (x_start + x_step*x_all_scale_count < v_max)
	{
		++x_all_scale_count;
	}

	small_grid_per_big_grid = units_per_big_grid;
	x_step_small = x_step / small_grid_per_big_grid;//小网格间隔
	{
		int x_step_small_px = isX ? TDraw::DPOINT2POINTXLEN(x_start, x_start + x_step_small, v_min, v_max, rcGraph) : TDraw::DPOINT2POINTYLEN(x_start, x_start + x_step_small, v_min, v_max, rcGraph);
		if (x_step_small_px > 20)
		{
			//缩小小网格

			while (x_step_small_px > 20)
			{
				switch (small_grid_per_big_grid)
				{
				case 1:small_grid_per_big_grid = 2; break;
				case 2:small_grid_per_big_grid = 5; break;
				default:small_grid_per_big_grid *= 2; break;
				}
				x_step_small = x_step / small_grid_per_big_grid;
				x_step_small_px = isX ? TDraw::DPOINT2POINTXLEN(x_start, x_start + x_step_small, v_min, v_max, rcGraph) : TDraw::DPOINT2POINTYLEN(x_start, x_start + x_step_small, v_min, v_max, rcGraph);
			}
		}
		else
			if (x_step_small_px < 10)
				bDrawSmallGrid = false;
		//else 4-20 维持原状
	}
}

//计算网格及刻度数据
//改变数据：
// vecGridLineAndScale, vecGridScaleSmall,rcLabelY, lScaleLeftWidth
//读取数据：
//x_min, x_max, y_min, y_max, sLabelY
void TGraph::CalcGridAndScale(HDC hdc, TPointData &PointData, LONG &lBottomSize, LONG &lRightSize,
	const RECT &rcGraph, LONG &lPrevAllWidth, LONG lScaleLong, LONG lInterval)
{
	//X网格参数
	int x_all_scale_count;
	int x_precision_digit(0);//有效数字结束处位于原数字的第几位 在小数点右边则为正
	double x_start;//x刻度开始位置
	double x_step;//每个大刻度间实际距离
	double x_step_small;
	int x_small_grid_per_big_grid;
	bool bDrawSmallGridX = true;

	CalcGridParameters(hdc, true, rcGraph, PointData.x_min, PointData.x_max,
		x_all_scale_count, x_precision_digit, x_start, x_step, x_step_small, x_small_grid_per_big_grid, bDrawSmallGridX);

	//Y网格参数
	int y_all_scale_count;
	int y_precision_digit(0);//有效数字结束处位于原数字的第几位 在小数点右边则为正
	double y_start;//x刻度开始位置
	double y_step;//每个大刻度间实际距离
	double y_step_small;
	int y_small_grid_per_big_grid;
	bool bDrawSmallGridY = true;

	CalcGridParameters(hdc, false, rcGraph, PointData.y_min, PointData.y_max,
		y_all_scale_count, y_precision_digit, y_start, y_step, y_step_small, y_small_grid_per_big_grid, bDrawSmallGridY);

	PointData.vecGridLineAndScale.clear();
	PointData.vecGridScaleSmall.clear();

	PointData.vecGridLineAndScale.reserve(x_all_scale_count * 2);
	PointData.vecGridScaleSmall.reserve((x_all_scale_count * 2) * 10);

	TGridLineAndScale GridLineAndScale;
	TGridScaleSmall GridScaleSmall;

	TCHAR szScale[64];
	DPOINT dpt, dptSmall;

	//存入X网格数据
	lBottomSize = 0;
	lRightSize = 0;
	for (int i = 0; i < x_all_scale_count + 1; ++i)
	{
		dpt.x = x_start + x_step*i;
		GridLineAndScale.ptScaleEnd = GridLineAndScale.ptScaleBegin = TDraw::DPOINT2POINT(dpt, PointData.x_min, PointData.x_max, PointData.y_min, PointData.y_max, rcGraph);
		GridLineAndScale.ptScaleBegin.y = rcGraph.bottom;
		GridLineAndScale.ptScaleEnd.y = GridLineAndScale.ptScaleBegin.y + lScaleLong;
		//TDraw::DrawLine(hdc, ptBegin, ptEnd, pConfig->logpenBlack);//画刻度线

		//画刻度文字
		TTransfer::double2TCHAR(dpt.x, szScale, x_precision_digit);
		GridLineAndScale.sScale = szScale;
		GridLineAndScale.rcScaleText = { 0, 0, 0, 0 };
		TDraw::DrawSystemFontText(hdc, szScale, GridLineAndScale.rcScaleText, 0, DT_CALCRECT);

		GridLineAndScale.rcScaleText.left = GridLineAndScale.ptScaleEnd.x - GridLineAndScale.rcScaleText.right / 2;
		GridLineAndScale.rcScaleText.right = GridLineAndScale.ptScaleEnd.x + GridLineAndScale.rcScaleText.right / 2;
		GridLineAndScale.rcScaleText.top = GridLineAndScale.ptScaleEnd.y + lInterval;
		GridLineAndScale.rcScaleText.bottom = GridLineAndScale.rcScaleText.top + GridLineAndScale.rcScaleText.bottom;
		//TDraw::DrawSystemFontText(hdc, szScaleX, rcScaleX, 0, DT_CENTER);

		//画大网格线
		GridLineAndScale.ptGridStart = GridLineAndScale.ptScaleEnd;
		GridLineAndScale.ptGridEnd.x = GridLineAndScale.ptScaleBegin.x;
		GridLineAndScale.ptGridEnd.y = rcGraph.top;
		//TDraw::DrawLine(hdc, ptGridBigTop, ptBegin, pConfig->logpenGraphGridBig);

		//画小网格线
		if (bDrawSmallGridX)
			for (int j = 1; j < x_small_grid_per_big_grid; ++j)
			{
				dptSmall.x = dpt.x + x_step_small*j;
				GridScaleSmall.ptEnd = GridScaleSmall.ptBegin = TDraw::DPOINT2POINT(dptSmall, PointData.x_min, PointData.x_max, PointData.y_min, PointData.y_max, rcGraph);
				if (GridScaleSmall.ptBegin.x<rcGraph.left || GridScaleSmall.ptBegin.x>rcGraph.right)
					continue;
				GridScaleSmall.ptBegin.y = rcGraph.top;
				GridScaleSmall.ptEnd.y = rcGraph.bottom;
				//TDraw::DrawLine(hdc, ptGridSmallTop, ptGridSmallBottom, pConfig->logpenGraphGridSmall);
				PointData.vecGridScaleSmall.push_back(GridScaleSmall);
			}

		//大网格若超出范围则不画
		if (GridLineAndScale.ptScaleBegin.x<rcGraph.left || GridLineAndScale.ptScaleBegin.x>rcGraph.right)
			continue;

		LONG newBottomSize = GridLineAndScale.rcScaleText.bottom - rcGraph.bottom;
		if (newBottomSize > lBottomSize) lBottomSize = newBottomSize;

		LONG newRightSize = GridLineAndScale.rcScaleText.right - rcGraph.right;
		if (newRightSize > lRightSize) lRightSize = newRightSize;

		PointData.vecGridLineAndScale.push_back(GridLineAndScale);
	}

	//存入Y网格数据
	PointData.lScaleLeftWidth = 0;
	for (int i = 0; i < y_all_scale_count + 1; ++i)
	{
		dpt.y = y_start + y_step*i;
		GridLineAndScale.ptScaleEnd = GridLineAndScale.ptScaleBegin = TDraw::DPOINT2POINT(dpt, PointData.x_min, PointData.x_max, PointData.y_min, PointData.y_max, rcGraph);
		GridLineAndScale.ptScaleBegin.x = rcGraph.left;
		GridLineAndScale.ptScaleEnd.x = GridLineAndScale.ptScaleBegin.x - lScaleLong;
		//TDraw::DrawLine(hdc, ptBegin, ptEnd, pConfig->logpenBlack);//画刻度线

		//画刻度文字
		TTransfer::double2TCHAR(dpt.y, szScale, y_precision_digit);
		GridLineAndScale.sScale = szScale;
		GridLineAndScale.rcScaleText = { 0, 0, 0, 0 };
		POINT ptScaleTextSize = TDraw::GetSystemFontSize(hdc, szScale);

		GridLineAndScale.rcScaleText.right = GridLineAndScale.ptScaleEnd.x - lInterval;
		GridLineAndScale.rcScaleText.left = GridLineAndScale.rcScaleText.right - ptScaleTextSize.x;
		GridLineAndScale.rcScaleText.top = GridLineAndScale.ptScaleEnd.y - ptScaleTextSize.y / 2;
		GridLineAndScale.rcScaleText.bottom = GridLineAndScale.ptScaleEnd.y + ptScaleTextSize.y / 2;
		//TDraw::DrawSystemFontTeyt(hdc, szScaley, rcScaley, 0, DT_CENTER);


		//画大网格线
		GridLineAndScale.ptGridStart.x = rcGraph.left;
		GridLineAndScale.ptGridStart.y = GridLineAndScale.ptScaleBegin.y;
		GridLineAndScale.ptGridEnd.y = GridLineAndScale.ptScaleBegin.y;
		GridLineAndScale.ptGridEnd.x = rcGraph.right;
		GridLineAndScale.ptGridEnd.y = GridLineAndScale.ptScaleBegin.y;
		//TDraw::DrawLine(hdc, ptGridBigTop, ptBegin, pConfig->logpenGraphGridBig);


		//画小网格线
		if (bDrawSmallGridY)
			for (int j = 1; j < y_small_grid_per_big_grid; ++j)
			{
				dptSmall.y = dpt.y + y_step_small*j;
				GridScaleSmall.ptEnd = GridScaleSmall.ptBegin = TDraw::DPOINT2POINT(dptSmall, PointData.x_min, PointData.x_max, PointData.y_min, PointData.y_max, rcGraph);
				if (GridScaleSmall.ptBegin.y<rcGraph.top || GridScaleSmall.ptBegin.y>rcGraph.bottom)
					continue;
				GridScaleSmall.ptBegin.x = rcGraph.left;
				GridScaleSmall.ptEnd.x = rcGraph.right;
				//TDraw::DrawLine(hdc, ptGridSmallTop, ptGridSmallBottom, pConfig->logpenGraphGridSmall);

				PointData.vecGridScaleSmall.push_back(GridScaleSmall);
			}

		//大网格若超出范围则不画
		if (GridLineAndScale.ptScaleBegin.y<rcGraph.top || GridLineAndScale.ptScaleBegin.y>rcGraph.bottom)
			continue;

		LONG newLeftSize = rcGraph.left - GridLineAndScale.rcScaleText.left;
		if (newLeftSize > PointData.lScaleLeftWidth) PointData.lScaleLeftWidth = newLeftSize;

		//向左移动
		//GridLineAndScale.ptGridEnd.x -= lPrevAllWidth;
		GridLineAndScale.ptScaleBegin.x -= lPrevAllWidth;
		GridLineAndScale.ptScaleEnd.x -= lPrevAllWidth;
		TDraw::MoveRect(GridLineAndScale.rcScaleText, lPrevAllWidth, 0);

		PointData.vecGridLineAndScale.push_back(GridLineAndScale);

		PointData.ptAxisBegin.x = rcGraph.left - lPrevAllWidth;
		PointData.ptAxisBegin.y = rcGraph.top;
		PointData.ptAxisEnd.x = rcGraph.left - lPrevAllWidth;
		PointData.ptAxisEnd.y = rcGraph.bottom;
	}

	//单位文字
	TDraw::DrawSystemFontText(hdc, PointData.sLabelY.c_str(), PointData.rcLabelY, 0, DT_CALCRECT);
	PointData.rcLabelY.top = rcGraph.top;
	PointData.rcLabelY.bottom = rcGraph.bottom;// +PointData.rcLabelY.right;
	PointData.rcLabelY.right = rcGraph.left - PointData.lScaleLeftWidth - lInterval;
	PointData.rcLabelY.left = PointData.rcLabelY.right - lTextHeight - lInterval;
	TDraw::MoveRect(PointData.rcLabelY, lPrevAllWidth, 0);

	lPrevAllWidth += PointData.lScaleLeftWidth +lTextHeight+ lInterval*4;
}

void TGraph::OnDraw(HDC hdc)
{
	if (!bDraw)
		return;

	SetBkMode(hdc, TRANSPARENT);

	//填充大背景
	TDraw::FillRect(hdc, &ClientRect, pConfig->crGraphBackground);

	//填充图背景-白色
	TDraw::FillRect(hdc, &(rcGraph), RGB(255, 255, 255));

	//标题
	if (bShowTitle)
	{
		RECT rcTitle;
		rcTitle.left = rcGraph.left;
		rcTitle.right = rcGraph.right;
		rcTitle.bottom = rcGraph.top - lInterval;
		rcTitle.top = rcTitle.bottom - lTextHeight - lInterval;
		TDraw::DrawSystemFontText(hdc, TEXT("Measured Results"), rcTitle, 0, DT_CENTER | DT_BOTTOM);
	}

	//scale
	DrawGridAndScale(hdc);

	//画黑框
	TDraw::DrawRect(hdc, TDraw::GetMarginRect(rcGraph, -1), pConfig->logpenBlack);

	//x坐标单位
	if (bShowLabelX)
	{
		RECT rcUnitsX;
		rcUnitsX.left = rcGraph.left;
		rcUnitsX.right = rcGraph.right;
		rcUnitsX.top = rcGraph.bottom + lScaleBottomHeight + lInterval;
		rcUnitsX.bottom = rcUnitsX.top + lTextHeight + lInterval;
		TDraw::DrawSystemFontText(hdc, sLabelX.c_str(), rcUnitsX, 0, DT_CENTER | DT_TOP);
	}

	//y坐标单位
	//if (bShowLabelY)
	for (auto &PointData : vecPointData)
	{
		if (PointData.Show)
			TDraw::DrawSystemFontTextVertical(hdc, PointData.sLabelY.c_str(), PointData.rcLabelY, 0, DT_VCENTER | DT_SINGLELINE);
	}

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
		TDraw::DrawTips(hdc, ptMouse, ClientRect, szCoordinate, pConfig);

		//画方块
		TDraw::DrawPickSquare(hdc, vecPointData[iPickPointDataIndex].ptArray[iPick]);
	}

	//画图例
	if (bShowLegend)
		DrawLegend(hdc, pConfig->logpenBlack);

}

void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder)
{

	POINT ptUpperRight = { rcGraph.right - 10, rcGraph.top + 10 };//离边缘距离10
	LONG lAllTextWidth(0), lAllTextHeight(0);
	LONG lLineLong = 36;//图例示范线长度
	LONG lDistFromLineAndText = 4;//内外框间距
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
	LONG lInnerWidth = lLineLong + lDistFromLineAndText + lAllTextWidth;
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
	rcText.left = rcInner.left + lLineLong + lDistFromLineAndText;
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

bool TGraph::SaveToCSV(TCHAR szFileName[])
{
	Ofstream ofs(szFileName);

	if (!ofs)
		return false;

	bool bShareXData = true;
	size_t x_num;
	if (!vecPointData.empty())
	{
		x_num = vecPointData[0].dptVector.size();
		for (auto &PointData : vecPointData)
			if (x_num != PointData.dptVector.size())
			{
				bShareXData = false;
				break;
			}
	}
	else
		return true;

	size_t col = vecPointData.size();
	if (bShareXData)
	{
		ofs.imbue(std::locale("", std::locale::all ^ std::locale::numeric));

		ofs << TEXT("t,");
		for (auto &PointData : vecPointData)
			ofs << PointData.sLegend << TEXT(",");
		ofs << endl;

		for (size_t i = 0; i < x_num; ++i)
		{
			ofs << vecPointData[0].dptVector[i].x << TEXT(",");
			for (size_t j = 0; j < col; ++j)
			{
				ofs << vecPointData[j].dptVector[i].y << TEXT(",");
			}
			ofs << endl;
		}
	}
	else
		assert(0);

	ofs.close();
	return true;
}

void TGraph::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	HMENU hMenu = GetMenu(m_hWnd);
	switch (wmId)
	{
	case ID_SAVE_AS_PICTURE:
		TCHAR szFileNamePNG[MAX_PATH];
		if (SaveFileDialog(m_hWnd, szFileNamePNG, TEXT("PNG File\0*.png\0\0"), TEXT("png")))
		{
			if (TDraw::CaptureWindowToFile(m_hWnd, szFileNamePNG))
				MessageBox(NULL, TEXT("保存成功。"), TEXT(""), 0);
			else
				MessageBox(NULL, TEXT("保存失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	case ID_SAVE_AS_CSV:
		TCHAR szFileNameCSV[MAX_PATH];
		if (SaveFileDialog(m_hWnd, szFileNameCSV, TEXT("CSV File\0*.csv\0\0"), TEXT("csv")))
		{
			if (SaveToCSV(szFileNameCSV))
				MessageBox(NULL, TEXT("保存成功。"), TEXT(""), 0);
			else
				MessageBox(NULL, TEXT("保存失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	case ID_MENU_GRAPH_SHOW_TITLE:
	{
		bool bChecked = GetMenuState(hMenu, wmId, MF_BYCOMMAND) == MF_CHECKED;
		CheckMenuItem(hMenu, wmId, bChecked ? MF_UNCHECKED : MF_CHECKED);
		bShowTitle = !bChecked;
		Invalidate();
		break;
	}
	case ID_MENU_GRAPH_SHOW_GRID:
	{
		bool bChecked = GetMenuState(hMenu, wmId, MF_BYCOMMAND) == MF_CHECKED;
		CheckMenuItem(hMenu, wmId, bChecked ? MF_UNCHECKED : MF_CHECKED);
		for (auto &PointData : vecPointData)
			PointData.bShowGridBig = PointData.bShowGridSmall = !bChecked;
		Invalidate();
		break;
	}
	case ID_MENU_GRAPH_SHOW_LEGEND:
	{
		bool bChecked = GetMenuState(hMenu, wmId, MF_BYCOMMAND) == MF_CHECKED;
		CheckMenuItem(hMenu, wmId, bChecked ? MF_UNCHECKED : MF_CHECKED);
		bShowLegend = !bChecked;
		Invalidate();
		break;
	}
	default://设置某条线的显隐
		size_t index = wmId - ID_MENU_GRAPH_DATA_START;

		bool bChecked = GetMenuState(hMenu, wmId, MF_BYCOMMAND) == MF_CHECKED;

		if (index < vecPointData.size())
		{
			vecPointData[index].Show = !bChecked;
			CheckMenuItem(hMenu, wmId, bChecked ? MF_UNCHECKED : MF_CHECKED);
		}
		OnSize(0, 0);
		Invalidate();
		break;
	}
}