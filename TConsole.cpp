#pragma once
#include "TConsole.h"
#include "TSolve.h"

TConsole::TConsole()
{
}


TConsole::~TConsole()
{
}

void TConsole::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。
	Edit.bMultiLine = true;
	Edit.CreateEditEx(hWnd, 0, m_hInst);
	Edit.SetPos(ClientRect);
	Edit.SetDefaultGuiFont();
	Edit.SetVisible(true);


	TSolve Solve;
	Solve.SetHwnd(Edit.m_hWnd);
	Solve.Solute();
}