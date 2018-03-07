#include "tchar_head.h"
#include "ShowMessage.h"

#include <stdio.h>//_vsntprintf

#include <strsafe.h>//ShowErrorMsgBox

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



//
void ShowErrorMsgBox(LPTSTR lpszFunction, DWORD dwErrorCode)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s ß∞‹°£¥ÌŒÛ¬Î %d: %s"),
		lpszFunction, dwErrorCode, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("¥ÌŒÛ"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}