#pragma once
#include "tchar_head.h"
#include <vector>

class TMyString
{
public:
	TMyString();
	~TMyString();

	//替换全部 src源字符串，sub被替换内容，dest替换的内容
	//返回替换过的数量
	//src大小需自行保证，函数内不验证指针越界
	static int TMyString::Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);
	static void TMyString::ReplaceLoop(TCHAR *src, TCHAR *sub, TCHAR *dest);

	//按照指定分隔符切分字符串存入result，result中的元素是new出来的
	static void TMyString::Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub);

	static bool TMyString::isAlphaCharOrUnderline(TCHAR c);
	static bool TMyString::isNumberChar(TCHAR c);
	static bool TMyString::isVariableName(TCHAR *varname);
	static void TMyString::Trim(TCHAR *str);

	static void TMyString::ReleaseVectorTCHAR(std::vector<TCHAR *> &szVector);
};

