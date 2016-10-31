#pragma once
#include <Windows.h>

#include "resource.h"
#include "TMainWindow.h"
#include "TManageTool.h"
#include "TSelectTool.h"
#include "TLineTool.h"

extern TMainWindow win;

TManageTool::TManageTool()
{
	m_pCurrentTool = NULL;
	m_uiCurActiveTool = 0;
}


TManageTool::~TManageTool()
{
}

void TManageTool::SetCurActiveTool(UINT id)
{
	if (m_pCurrentTool != NULL)
	{
		delete m_pCurrentTool;
		m_pCurrentTool = NULL;
	}

	m_uiCurActiveTool = id;
	switch (id)
	{
	case ID_DRAW_LINE:
		m_pCurrentTool = new TLineTool;
		break;
	case ID_SELECT:
		m_pCurrentTool = new TSelectTool;
		break;
	}
}


void TManageTool::SetCursor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		POINT ptPos;

		ptPos.x = LOWORD(lParam);
		ptPos.y = HIWORD(lParam);
	if (m_pCurrentTool != NULL)
	{
		m_pCurrentTool->OnSetCursor(hWnd, message,ptPos);
	}
	else
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}

}


void TManageTool::Message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrentTool == NULL) return;

	POINT ptPos;

	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	switch (message)
	{
	case WM_KEYDOWN:
		m_pCurrentTool->OnKeyDown(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		m_pCurrentTool->OnMouseMove(hWnd, wParam, ptPos);
		break;
	case WM_LBUTTONDOWN:
		m_pCurrentTool->OnLButtonDown(hWnd, wParam, ptPos);
		break;
	case WM_RBUTTONDOWN:
		m_pCurrentTool->OnRButtonDown(hWnd, wParam, ptPos);
		break;
	case WM_MOUSEWHEEL:
		m_pCurrentTool->OnMouseWheel(hWnd, wParam, ptPos);
		break;
	}
}