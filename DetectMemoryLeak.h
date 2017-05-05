#ifdef _DEBUG

#include "tchar_head.h"
void OutputDebugPrintf(const TCHAR * strOutputString, ...);
void OutputDebugDouble(double d);
void OutputDebugInt(int i);

#pragma once
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define VCZH_CHECK_MEMORY_LEAKS_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new VCZH_CHECK_MEMORY_LEAKS_NEW

#include <assert.h>


#else
#define NDEBUG
#include <assert.h>
#endif