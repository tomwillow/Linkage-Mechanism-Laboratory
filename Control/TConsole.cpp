#pragma once
#include "..\resource.h"
#include "TConsole.h"
#include "..\Solver\TSolver.h"

TConsole::TConsole()
{
}


TConsole::~TConsole()
{
	pSolver->SetHwnd(NULL);
	DestroyWindow(m_hWnd);
}

void TConsole::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。
	Edit.bAutoVScrol = true;
	Edit.bMultiLine = true;
	Edit.bNoHideSel = true;
	Edit.CreateEditEx(hWnd, 0, m_hInst);
	Edit.SetPosition(ClientRect);
	Edit.SetDefaultGuiFont();
	Edit.SetVisible(true);


	pSolver->SetHwnd(Edit.m_hWnd);
	pSolver->RefreshWindowText();
	//pSolver->Demo();

	SetFocus(Edit.m_hWnd);
	Edit.SetSelect(Edit.GetLength(), Edit.GetLength());
	PostMessage(Edit.m_hWnd, EM_SCROLLCARET, 0, 0);
}

void TConsole::OnSize(WPARAM wParam, LPARAM lParam)
{
	Edit.SetPosition(ClientRect);
}

bool TConsole::OnClose()
{
	//ShowWindow(SW_HIDE);
	PostMessage(m_hParent, WM_COMMAND, ID_DELETE_CONSOLE, 0);
	return true;
}