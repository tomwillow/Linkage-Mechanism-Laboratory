#include "..\Common\tchar_head.h"

#include "..\Common\FileFunction.h"

#include <Windows.h>

//传入index=1则得到传入文件名
//成功取得返回true 
bool GetCommandLineByIndex(int index, TCHAR *assigned)
{
	int iCmdLineCount = -1;
	int len = _tcslen(GetCommandLine());
	TCHAR *origin = new TCHAR[len + 1];
	TCHAR *s = origin;
	_tcscpy(s, GetCommandLine());
	bool inchar = false;
	TCHAR *start, *end;
	while ((s = _tcschr(s, TEXT('\"'))) != NULL)
	{
		s++;
		if (inchar == false)//开端
		{
			start = s;
			inchar = true;
		}
		else
		{
			end = s - 1;
			iCmdLineCount++;
			if (iCmdLineCount == index)
			{
				_tcsncpy(assigned, start, end - start);
				assigned[end - start] = TEXT('\0');
				break;
			}
			inchar = false;
		}
	}
	delete[] origin;
	if (iCmdLineCount == index)
		return true;
	else
		return false;
}


bool GetFileExists(TCHAR filename[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(filename, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

//lpstrFilter格式：TEXT("机构设计文件(*.lml)\0*.lml\0\0")
//ofn不另设空间保存文件地址，改动szFile等同于改动ofn.lpstrFile
void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], TCHAR lpstrFilter[], DWORD nMaxFile)
{
	// Initialize OPENFILENAME
	ZeroMemory(ofn, sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->lpstrFile = szFile;
	ofn->lpstrFile[0] = TEXT('\0');
	ofn->nMaxFile = nMaxFile;
	ofn->lpstrFilter = lpstrFilter;//两个\0表示结束
	ofn->nFilterIndex = 1;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	ofn->lpstrInitialDir = NULL;
}

BOOL OpenFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[])
{
	OPENFILENAME ofn;
	InitialOpenFileName(&ofn, hWnd, szFileName, lpstrFilter);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//限定文件必须存在
	return GetOpenFileName(&ofn);
}


BOOL SaveFileDialog(HWND hWnd, TCHAR szFileName[], TCHAR lpstrFilter[],TCHAR lpstrDefExt[])
{
	OPENFILENAME ofn;
	InitialOpenFileName(&ofn, hWnd, szFileName, lpstrFilter);

	ofn.Flags = OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = lpstrDefExt;

	return GetSaveFileName(&ofn);
}