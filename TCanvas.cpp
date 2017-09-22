#pragma once
#include <Windows.h>
#include "tchar_head.h"
#include <stdio.h>

#pragma   comment(lib,"Msimg32.lib")//AlphaBlend使用

#include "resource.h"
#include "TConfiguration.h"
#include "TMainWindow.h"
#include "TCanvas.h"
#include "TManageTool.h"
#include "TLineTool.h"
#include "TDraw.h"

extern TMainWindow win;
TCanvas::TCanvas()
{
	pConfig = NULL;
	bMButtonPressing = false;
}


TCanvas::~TCanvas()
{
	pConfig = NULL;
}

void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	pConfig = &(win.m_Configuration);
	m_hWnd = hWnd;
}

void TCanvas::DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TManageTool *pManageTool = &(win.m_ManageTool);
	if (pManageTool->m_pCurrentTool != NULL)
	{
		pManageTool->Message(hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_SETCURSOR)
		pManageTool->SetCursor(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//热键返回给主窗口
	win.OnCommand(wParam, lParam);
}

void TCanvas::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	DealMessage(m_hWnd, WM_KEYDOWN, wParam, lParam);
}

void TCanvas::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
	Invalidate();
}

void TCanvas::OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnHotKey(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT ptPos;

	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	switch (pConfig->uUnits)
	{
	case UNITS_MM:
		win.m_Status.SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f mm,%.3f mm)"), pConfig->ScreenToReal(ptPos).x, pConfig->ScreenToReal(ptPos).y);
		break;
	case UNITS_INCH:
		win.m_Status.SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f in,%.3f in)"), pConfig->ScreenToReal(ptPos).x, pConfig->ScreenToReal(ptPos).y);
		break;
	}

	//若中键按下，拖动坐标原点并刷新显示
	if (bMButtonPressing)
	{
		pConfig->SetOrg(pConfig->GetOrg().x + (ptPos.x - uiMoveStart.x),
			pConfig->GetOrg().y + (ptPos.y - uiMoveStart.y));
		uiMoveStart = ptPos;
	}

	DealMessage(hWnd, uMsg, wParam, lParam);

	InvalidateRect(this->m_hWnd, &(this->ClientRect), false);
}

void TCanvas::OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bMButtonPressing)
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	else
		DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = true;
	uiMoveStart.x = LOWORD(lParam);
	uiMoveStart.y = HIWORD(lParam);
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = false;
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	POINT ptPos;
	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	//WM_MOUSEWHEEL事件获得的是相对于屏幕的绝对坐标
	ScreenToClient(hWnd, &ptPos);

	//首先取得鼠标位置到原点的实际距离，在比例变动后以相同的实际距离推断新的原点位置。
	//变动前后的依据是实际距离不变。
	double xlen = pConfig->ScreenToLengthX(ptPos.x - pConfig->GetOrg().x);
	double ylen = pConfig->ScreenToLengthY(ptPos.y - pConfig->GetOrg().y);

	if (zDelta > 0)
		pConfig->SetDPU(win.m_Trackbar.GetNextValue());
	else
		pConfig->SetDPU(win.m_Trackbar.GetPrevValue());

	pConfig->SetOrg(ptPos.x - pConfig->LengthToScreenX(xlen), ptPos.y - pConfig->LengthToScreenY(ylen));

	win.m_Trackbar.SetPosByValue(pConfig->GetProportion(), 1e-6);

	//因为原来也是0，SetPos后也是0，SetPos不激发NOTIFY，所以手动发
	PostMessage(m_hParent, WM_NOTIFY, MAKELONG(IDR_TRACKBAR, 0), 0);

	DealMessage(hWnd, uMsg, wParam, lParam);
	Invalidate();
}


void TCanvas::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//填充背景
	TDraw::FillRect(hdc, &ClientRect, pConfig->crBackground);

	//画网格
	if (pConfig->bDrawGrid)
		TDraw::DrawGrid(hdc, ClientRect, pConfig->GetOrg(), pConfig->crGridBig, pConfig->crGridSmall, pConfig);

	if (pConfig->bDrawAxes)//画坐标原点
		TDraw::DrawAxes(hdc, pConfig->GetOrg().x, pConfig->GetOrg().y, pConfig->crCoordinate);

	//图形绘制
	for (auto pElement : win.m_Shape.Element)
	{
		pElement->Draw(hdc, pConfig);
	}

	//工具类绘制
	if (win.m_ManageTool.m_pCurrentTool != NULL)
		win.m_ManageTool.m_pCurrentTool->Draw(hdc);//工具在使用中的图形绘制交由工具类执行

}
