#ifdef _DEBUG

#include <windows.h>
#include <stdarg.h>//va_start
#include <tchar.h>

void OutputDebugPrintf(const TCHAR * strOutputString, ...)
{
	TCHAR strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsntprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(strBuffer);
}

#endif