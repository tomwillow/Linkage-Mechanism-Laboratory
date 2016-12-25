#pragma once
#include "TWindow.h"
#include "TEdit.h"

class TConsole :
	public TWindow
{
private:
	TEdit Edit;
	void TConsole::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TConsole::OnSize(WPARAM wParam, LPARAM lParam);
public:
	TConsole();
	~TConsole();
};

