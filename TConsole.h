#pragma once
#include "TWindow.h"
#include "TEdit.h"

class TSolver;
class TConsole :
	public TWindow
{
private:
	TEdit Edit;
	void TConsole::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TConsole::OnSize(WPARAM wParam, LPARAM lParam);
	bool TConsole::OnClose();
public:
	TSolver *pSolver;
	TConsole();
	~TConsole();
};

