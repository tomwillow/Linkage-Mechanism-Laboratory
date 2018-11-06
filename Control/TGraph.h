#pragma once
#include <vector>
#include <Windows.h>

#include "TWindow.h"
#include "..\Common\DPOINT.h"
#include "..\Element\TLine.h"

	using std::vector;

class TConfiguration;
class TGraph :
	public TWindow
{
private:
	//double x_max=21.3703, x_min=21.202, y_max=0.0, y_min=0.0;

	//大网格线及刻度数据类型
	struct TGridLineAndScale
	{
		POINT ptGridStart;//大网格线起点
		POINT ptGridEnd;
		POINT ptScaleBegin;//刻度起点
		POINT ptScaleEnd;//刻度终点
		RECT rcScaleText;//刻度文字包围盒
		String sScale;
	};

	//小网格线数据类型
	struct TGridScaleSmall
	{
		POINT ptBegin;//小网格线起点
		POINT ptEnd;//小网格线终点
	};

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
		std::vector<TGridLineAndScale> vecGridLineAndScale;//大网格线及刻度线集合
		std::vector<TGridScaleSmall> vecGridScaleSmall;//小网格线集合
		LONG lScaleLeftWidth;//刻度+间隔+刻度文字 总宽度
		String sLabelY;
		bool bShowGridBig = false;
		bool bShowGridSmall = false;
		RECT rcLabelY;

		POINT ptAxisBegin, ptAxisEnd;
	};
		LONG lScaleBottomHeight;//刻度+间隔+刻度文字 总高度

	std::vector<TPointData> vecPointData;

	RECT rcGraph;//在窗口内的显示范围

	TConfiguration *pConfig;
	TLine LineMouseX, LineMouseY;
	POINT ptMouse;
	DPOINT dptMouse;
	int iPickPointDataIndex;
	int iPick;

	int iMarginTop,iMarginBottom,iMarginLeft,iMarginRight;
	LONG lTextHeight;
	LONG lInterval=2;//刻度与刻度文字间隔


	HMENU hMenuData;

	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnCommand(WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray(const RECT &rcGraph);
	void TGraph::DrawLegend(HDC hdc, const LOGPEN &logpenBorder); 
	void TGraph::DrawGridAndScale(HDC hdc);
	bool TGraph::SaveToCSV(TCHAR szFileName[]);

	void TGraph::CalcGridAndScale(HDC hdc, TPointData &PointData, LONG &lBottomSize, LONG &lRightSize,
		const RECT &rcGraph, LONG &lPrevAllWidth, LONG lScaleLong, LONG lInterval);
	void TGraph::AttachPoint();
public:
	bool bDraw = true;//如果为false，则OnDraw事件中不动作
	bool bRealClose = true;//如果为true，则关闭时关闭窗口。否则只隐藏窗口
	bool bShowMouseLine = false;
	bool bAdaptiveMargin = true;
	bool bShowTitle = true;
	bool bShowLabelX = true;
	bool bShowLegend = true;//显示图例

	//x,y轴标签
	String sLabelX;

	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<double> &vecX, const std::vector<double> &vecY, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""), const TCHAR szUnitLabel[] = TEXT(""));
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector, const LOGPEN &logpen, bool visible, const TCHAR szLegend[] = TEXT(""),const TCHAR szUnitLabel[]=TEXT(""));
	void TGraph::Clear();
	void TGraph::Refresh();//刷新 等同于调用OnSize
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};