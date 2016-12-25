#pragma once
#include "TConsole.h"
#include "TSolver.h"

TConsole::TConsole()
{
}


TConsole::~TConsole()
{
}

void TConsole::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。
	Edit.bAutoVScrol = true;
	Edit.bMultiLine = true;
	Edit.bNoHideSel = true;
	Edit.CreateEditEx(hWnd, 0, m_hInst);
	Edit.SetPos(ClientRect);
	Edit.SetDefaultGuiFont();
	Edit.SetVisible(true);


	TSolver Solver;
	Solver.SetHwnd(Edit.m_hWnd);
	Solver.Solute(true);

	SetFocus(Edit.m_hWnd);
	//Edit.SetSelectAll();
	Edit.SetSelect(Edit.GetLength(), Edit.GetLength());
	PostMessage(Edit.m_hWnd, EM_SCROLLCARET, 0, 0);
}

void TConsole::OnSize(WPARAM wParam, LPARAM lParam)
{
	Edit.SetPos(ClientRect);
}