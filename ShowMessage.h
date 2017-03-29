#pragma once
#include <Windows.h>
#include <tchar.h>

void ShowMessage(TCHAR szFormat[], ...);
int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid);