#pragma once
#include "TWindow.h"
#include "resource.h"
#include "TRightWindow.h"
#include "TConfiguration.h"
#include "TToolbar.h"
#include "TManageTool.h"
#include "TCanvas.h"
#include "TShape.h"
#include "TStatus.h"
#include "TTrackbar.h"
#include "TEdit.h"

class TMainWindow: public TWindow
{
private:
	TCHAR szFileName[MAX_PATH];
	int m_iRightWindowWidth;
	void TMainWindow::SetCanvasPos();
	void TMainWindow::SetRightWindowPos();
public:
	TCanvas Canvas;
	TRightWindow RightWindow;
	TManageTool m_ManageTool;
	TShape m_Shape;
	TConfiguration m_Configuration;
	TToolbar m_Toolbar;
	TStatus m_Status;
	TTrackbar m_Trackbar;
	TMainWindow();
	~TMainWindow();

	void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};