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
	struct TPointData
	{
		bool Show;
		std::vector<DPOINT> dptVector;
		int iPtCount;
		POINT *ptArray;
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
	void TGraph::OnSize(WPARAM wParam, LPARAM lParam);
	void TGraph::OnDraw(HDC hdc);
	bool TGraph::OnClose();
	void TGraph::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TGraph::CalcPointArray();
public:
	bool bShowMouseLine = false;
	TGraph(TConfiguration *pConfig);
	~TGraph();
	void TGraph::InputDptVector(const std::vector<DPOINT> &dptInputVector,const LOGPEN &logpen,bool visible);
	void TGraph::Clear();
	void TGraph::AttachPoint();
	void TGraph::SetMargin(int iMargin);
	void TGraph::SetPointDataVisible(int index,bool visible);
};

