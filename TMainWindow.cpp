#pragma once
#include "TMainWindow.h"

TMainWindow::TMainWindow()
{
}


TMainWindow::~TMainWindow()
{
}

void TMainWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。

	//配置初始化
	this->m_Configuration.Initial(hWnd);

	//创建工具栏
	m_Toolbar.bAutoWrap = true;
	m_Toolbar.bIsFlat = true;
	m_Toolbar.CreateToolbar(hWnd, m_hInst);
	m_Toolbar.LoadImageList(16, 16, IDR_TOOLBAR_MAIN);
	m_Toolbar.AddButton(0, ID_NEW, true, TEXT("New"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddCheck(1, ID_NEW, true, NULL);
	m_Toolbar.AddGroup(2, ID_NEW, true, NULL);
	m_Toolbar.AddGroup(3, ID_NEW, true, NULL);
	m_Toolbar.ShowToolbar();

	//创建状态栏
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);
	m_Status.AddPart(0, 6, PT_FIXED);
	m_Status.FreshSize();

	m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："), m_Configuration.GetProportion() * 100);
	m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);

	//创建Trackbar
	m_Trackbar.CreateTrackbar(m_Status.m_hWnd, this->m_hInst, m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0), IDR_TRACKBAR);
	m_Trackbar.SetRange(6);
	m_Trackbar.SetPos(3);

	//创建画布
	int x, y, width, height;
	x = ClientRect.left;
	y = ClientRect.top + m_Toolbar.GetClientRect().bottom;
	width = ClientRect.right - ClientRect.left;
	height = ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom;
	Canvas.CreateEx(0, TEXT("canvas"), TEXT("canvas"),
		WS_CHILD,// | WS_VISIBLE
		x,
		y,
		width,
		height,//
		hWnd,
		(HMENU)ID_CANVAS,//id
		m_hInst);
	Canvas.SetDoubleBuffer(true);
	Canvas.ShowWindow(TRUE);
	Canvas.UpdateWindow();

	//TEdit edit;
	//edit.CreateEditEx(Canvas.m_hWnd, IDR_LINEEDIT, m_hInst);
	//edit.SetDoubleBuffer(true);
	//edit.SetPos(0, 0, 40, 20);
	//edit.ShowWindow(true);
	//edit.UpdateWindow();

	//默认初始化
	this->OnCommand(MAKELONG(ID_NEW, 0), 0);

}


void TMainWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	if (wmId == IDR_TRACKBAR)
	{
		//拖动了Trackbar
		switch (m_Trackbar.GetPos())
		{
		case 0:
			m_Configuration.SetDPU(0.125); break;
		case 1:
			m_Configuration.SetDPU(0.25); break;
		case 2:
			m_Configuration.SetDPU(0.5); break;
		case 3:
			m_Configuration.SetDPU(1.0); break;
		case 4:
			m_Configuration.SetDPU(2.0); break;
		case 5:
			m_Configuration.SetDPU(4.0); break;
		case 6:
			m_Configuration.SetDPU(8.0); break;
		}
		m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："), m_Configuration.GetProportion() * 100);
		m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);
		InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, false);
	}
}

void TMainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	TManageTool *p_Managetool = &m_ManageTool;
	switch (wmId)
	{
	case ID_ACCELERATOR_NEW:
	case ID_NEW:
		//MessageBeep(0);
		this->m_Configuration.Org.x = this->Canvas.ClientRect.right / 2;
		this->m_Configuration.Org.y = this->Canvas.ClientRect.bottom / 2;

		break;
	case ID_EXIT:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case ID_SELECT:
		p_Managetool->SetCurActiveTool(ID_SELECT);
		break;
	case ID_DRAW_LINE:
		p_Managetool->SetCurActiveTool(ID_DRAW_LINE);
		break;
	}
}

void TMainWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	m_Toolbar.FreshSize();
	m_Status.FreshSize();
	m_Trackbar.MoveWindow(m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0));

	//记录下坐标原点相对比例，Canvas变动后再按比例恢复坐标位置
	DPOINT OrgProportion;
	OrgProportion.x = (double)m_Configuration.Org.x / (double)Canvas.ClientRect.right;
	OrgProportion.y = (double)m_Configuration.Org.y / (double)Canvas.ClientRect.bottom;

	MoveWindow(Canvas.m_hWnd,
		ClientRect.left,
		ClientRect.top + m_Toolbar.GetClientRect().bottom,
		ClientRect.right - ClientRect.left,
		ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom, true);

	m_Configuration.Org.x = (LONG)(OrgProportion.x*Canvas.ClientRect.right);
	m_Configuration.Org.y = (LONG)(OrgProportion.y*Canvas.ClientRect.bottom);

	InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, FALSE);
}
