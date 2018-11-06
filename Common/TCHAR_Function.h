#pragma once
#include "..\Common\tchar_head.h"
#include <vector>

class TCHAR_Function
{
public:
	TCHAR_Function();
	~TCHAR_Function();

	//替换全部 src源字符串，sub被替换内容，dest替换的内容
	//返回替换过的数量
	//src大小需自行保证，函数内不验证指针越界
	static int TCHAR_Function::Replace(TCHAR *src, TCHAR *sub, TCHAR *dest);
	static void TCHAR_Function::ReplaceLoop(TCHAR *src, TCHAR *sub, TCHAR *dest);

	//按照指定分隔符切分字符串存入result，result中的元素是new出来的
	static void TCHAR_Function::Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub);

	static bool TCHAR_Function::isAlphaCharOrUnderline(TCHAR c);
	static bool TCHAR_Function::isNumberChar(TCHAR c);
	static bool TCHAR_Function::isVariableName(TCHAR *varname);
	static void TCHAR_Function::Trim(TCHAR *str);

	static void TCHAR_Function::ReleaseVectorTCHAR(std::vector<TCHAR *> &szVector);
};

