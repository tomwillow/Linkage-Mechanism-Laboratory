#pragma once
#include <vector>
#include <Windows.h>

#include "TWindow.h"
#include "DPOINT.h"
#include "TLine.h"
class TConfiguration;
class TGraph :
	public TWindow
{
private:
	double x_max=21.3703, x_min=21.202, y_max=0.0, y_min=0.0;
	struct TPointData
	{
		bool Show;
		std::vector<DPOINT> dptVector;//真实数据
		String sLegend;
		LONG lLegendWidth, lLegendHeight;
		int iPtCount;//点数量
		POINT *ptArray;//坐标点
		double x_max, y_max, x_min, y_min;
		double x_len, y_len;
		LOGPEN logpen;
	};

	TConfiguration *pConfig;
	std::vector<TPointData> vecPointData;
	TLine LineMouseX, LineMouseY;
	POINT ptMouse;
	DPOINT dptMouse;
	RECT rcGraph;//在窗口内的显示范围
	int iPickPointDataIndex;
	int iPick;

	int iMargin;

	struct TGridLineAndScale
	{
		POINT ptGridUp;
		POINT ptScaleBegin;
		POINT ptScaleEnd;
		RECT rcScaleText;
		String szScale;
	};
	std::vector<TGridLineAndScale> vecGridLineAndScaleX;

	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray(const RECT &rcGraph);
	void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder);
	void TGraph::DrawGridAndScale(HDC hdc);
public:
	bool bRealClose = true;
	bool bShowMouseLine = false;
	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[]=TEXT(""));
	void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[]=TEXT(""));
	void TGraph::Clear();
	void TGraph::AttachPoint();
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};

