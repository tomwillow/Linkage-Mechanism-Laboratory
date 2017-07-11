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
		LONG lLegendWidth, lLegendHeight;//对应图例的宽高
		int iPtCount;//点数量
		POINT *ptArray;//坐标点
		double x_max, y_max, x_min, y_min;
		double x_len, y_len;
		LOGPEN logpen;
	};
	std::vector<TPointData> vecPointData;

	TConfiguration *pConfig;
	TLine LineMouseX, LineMouseY;
	POINT ptMouse;
	DPOINT dptMouse;
	RECT rcGraph;//在窗口内的显示范围
	int iPickPointDataIndex;
	int iPick;

	int iMarginTop,iMarginBottom,iMarginLeft,iMarginRight;
	LONG lTextHeight;
	LONG lInterval=2;//刻度与刻度文字间隔
	LONG lScaleBottomHeight;//刻度+间隔+刻度文字 总高度
	LONG lScaleLeftWidth;//刻度+间隔+刻度文字 总宽度

	struct TGridLineAndScale
	{
		POINT ptGridStart;
		POINT ptScaleBegin;
		POINT ptScaleEnd;
		RECT rcScaleText;
		String sScale;
	};
	std::vector<TGridLineAndScale> vecGridLineAndScale;//大网格线及刻度线集合

	struct TGridScaleSmall
	{
		POINT ptBegin;
		POINT ptEnd;
	};
	std::vector<TGridScaleSmall> vecGridScaleSmall;//小网格线集合

	HMENU hMenuData;

	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnCommand(WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray(const RECT &rcGraph);
	void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder);
	void TGraph::CalcGridAndScale(HDC hdc, LONG &lBottomSize, LONG &lLeftSize,LONG &lRightSize, LONG lScaleLong, LONG lInterval);
	void TGraph::DrawGridAndScale(HDC hdc);
	bool TGraph::SaveToCSV(TCHAR szFileName[]);
public:
	bool bDraw = true;//如果为false，则OnDraw事件中不动作
	bool bRealClose = true;//如果为true，则关闭时关闭窗口。否则只隐藏窗口
	bool bShowMouseLine = false;
	bool bAdaptiveMargin = true;
	bool bShowTitle = true;
	bool bShowLabelX = true;
	bool bShowLabelY = true;
	bool bShowLegend = true;//显示图例
	bool bShowGridBig = true;
	bool bShowGridSmall = true;

	//x,y轴标签
	String sLabelX;
	String sLabelY;

	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""));
	void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""));
	void TGraph::Clear();
	void TGraph::Refresh();//刷新 等同于调用OnSize
	void TGraph::AttachPoint();
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};

