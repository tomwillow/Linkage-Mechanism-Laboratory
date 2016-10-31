#pragma once
#include "KWindow.h"
#include "resource.h"
#include "TConfiguration.h"
#include "TToolbar.h"
#include "TManageTool.h"
#include "TCanvas.h"
#include "TShape.h"
#include "TStatus.h"
#include "TTrackbar.h"
#include "TEdit.h"

class TMainWindow: public KWindow
{
public:
	TCanvas Canvas;
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
	void OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		MessageBeep(0);
	};
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};