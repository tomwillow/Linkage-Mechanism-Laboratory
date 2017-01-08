#pragma once
#include "TManageTool.h"

#include "resource.h"

#include "TSelectTool.h"
#include "TBarTool.h"
#include "TLineTool.h"
#include "TFramePointTool.h"

TManageTool::TManageTool()
{
	bReceiveMsg = true;
	m_pCurrentTool = NULL;
	m_uiCurActiveTool = 0;
}


TManageTool::~TManageTool()
{
}

void TManageTool::CloseCurTool()
{
	if (m_pCurrentTool != NULL)
	{
		m_pCurrentTool->~TTool();
		delete m_pCurrentTool;
		m_pCurrentTool = NULL;
	}
}

void TManageTool::SetCurActiveTool(UINT id)
{
	CloseCurTool();

	m_uiCurActiveTool = id;
	switch (id)
	{
	case ID_SELECT:
		m_pCurrentTool = new TSelectTool;
		break;
	case ID_DRAW_FRAME:
		m_pCurrentTool = new TFramePointTool;
		break;
	case ID_DRAW_BAR:
		m_pCurrentTool = new TBarTool;
		break;
	case ID_DRAW_LINE:
		m_pCurrentTool = new TLineTool;
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
		m_pCurrentTool->OnSetCursor(hWnd, message, ptPos);
	}
	else
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}

}


void TManageTool::Message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (bReceiveMsg == false) return;
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