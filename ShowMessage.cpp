#include "ShowMessage.h"

#include <stdio.h>//_vsntprintf

void ShowMessage(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	MessageBox(0, szBuffer, TEXT(""), 0);
}

int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid)
{
	MSGBOXPARAMS param;

	memset(&param, 0, sizeof(param));
	param.cbSize = sizeof(param);
	param.hwndOwner = hWnd;
	param.hInstance = GetModuleHandle(NULL);
	param.lpszText = text;
	param.lpszCaption = caption;
	param.dwStyle = style | MB_USERICON;
	param.lpszIcon = MAKEINTRESOURCE(iconid);

	return MessageBoxIndirect(&param);
}
