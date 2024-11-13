#include "..\Common\tchar_head.h"
#include "..\Common\ShowMessage.h"

#include <stdio.h>//_vsntprintf

#include <strsafe.h>//ShowErrorMsgBox

void ShowMessage(const TCHAR szFormat[], ...)
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


bool IsErrorShowMsgBox(LPTSTR lpszFunction)
{
	DWORD dw = GetLastError();
	if (dw != 0)
	{
		ShowErrorMsgBox(lpszFunction, dw);
		return true;
	}
	else
		return false;
}

//根据错误码弹出错误信息框
//lpszFunction为出错步骤名称
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
		TEXT("%s失败。错误码 %d: %s"),
		lpszFunction, dwErrorCode, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("错误"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}