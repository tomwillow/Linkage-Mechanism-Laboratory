#pragma once

#include <tchar.h>
#include <Windows.h>

bool GetCommandLineByIndex(int index, TCHAR *assigned);
bool GetFileExists(TCHAR filename[]);
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], DWORD nMaxFile = MAX_PATH);