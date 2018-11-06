#pragma once
#include "..\Common\DetectMemoryLeak.h"
#include "..\Tool\TManageTool.h"

#include "..\resource.h"

#include "..\Tool\TSelectTool.h"
#include "TDragTool.h"
#include "TBarTool.h"
#include "TLineTool.h"
#include "TFramePointTool.h"
#include "TSliderTool.h"
#include "TSlidewayTool.h"
#include "TPolylineBarTool.h"

#include "TConstraintCoincideTool.h"
#include "TConstraintColinearTool.h"

TManageTool::TManageTool()
{
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
	UINT uiPrevTool = m_uiCurActiveTool;
	CloseCurTool();

	m_uiCurActiveTool = id;
	switch (id)
	{
	case ID_SELECT:
		m_pCurrentTool = new TSelectTool;
		break;
	case ID_DRAG:
		m_pCurrentTool = new TDragTool;
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
	case ID_DRAW_SLIDER:
		m_pCurrentTool = new TSliderTool;
		break;
	case ID_DRAW_SLIDEWAY:
		m_pCurrentTool = new TSlidewayTool;
		break;
	case ID_DRAW_POLYLINE_BAR:
		m_pCurrentTool = new TPolylineBarTool;
		break;
	case ID_DRAW_COINCIDE:
		m_pCurrentTool = new TConstraintCoincideTool;
		break;
	case ID_DRAW_COLINEAR:
		m_pCurrentTool = new TConstraintColinearTool;
		break;
	default:
		assert(0);
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
	case WM_LBUTTONUP:
		m_pCurrentTool->OnLButtonUp(hWnd, wParam, ptPos);
		break;
	case WM_RBUTTONDOWN:
		m_pCurrentTool->OnRButtonDown(hWnd, wParam, ptPos);
		break;
	case WM_RBUTTONUP:
		m_pCurrentTool->OnRButtonUp(hWnd, wParam, ptPos);
		break;
	case WM_HOTKEY:
		m_pCurrentTool->OnHotKey(hWnd, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		m_pCurrentTool->OnMouseWheel(hWnd, wParam, ptPos);
		break;
	}
}