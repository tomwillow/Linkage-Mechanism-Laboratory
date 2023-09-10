#pragma once

#include <tchar.h>
#include <Windows.h>

bool GetCommandLineByIndex(int index, TCHAR *assigned);
bool GetFileExists(TCHAR filename[]);
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], TCHAR lpstrFilter[], DWORD nMaxFile= MAX_PATH);


//szFileName保存文件名的位置
//lpstrFilter示例： TEXT("机构设计文件(*.lml)\0*.lml\0\0")
BOOL OpenFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[]);

//szFileName保存文件名的位置
//lpstrFilter示例： TEXT("机构设计文件(*.lml)\0*.lml\0\0")
//lpstrDefExt示例： TEXT("lml")
BOOL SaveFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[], TCHAR lpstrDefExt[]);
