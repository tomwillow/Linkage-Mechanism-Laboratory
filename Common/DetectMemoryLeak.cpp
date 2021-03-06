#ifdef _DEBUG

#include "..\Common\DetectMemoryLeak.h"
#include <windows.h>
#include <stdio.h>//_vsntprintf
#include <stdarg.h>//va_start

void OutputDebugPrintf(const TCHAR * strOutputString, ...)
{
	TCHAR strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsntprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(strBuffer);
}

void OutputDebugDouble(double d)
{
	OutputDebugPrintf(TEXT("%f\n"), d);
}

void OutputDebugInt(int i)
{
	OutputDebugPrintf(TEXT("%d\n"), i);
}

#endif