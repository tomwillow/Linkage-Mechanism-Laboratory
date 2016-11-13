#pragma once
#include <Windows.h>

#include "TTool.h"


class TManageTool
{
public:
	UINT m_uiCurActiveTool;
	TTool *m_pCurrentTool;

	TManageTool();
	~TManageTool();

	void SetCurActiveTool(UINT id);
	void Message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void SetCursor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

