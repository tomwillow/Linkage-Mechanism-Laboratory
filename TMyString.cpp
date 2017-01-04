#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "TMyString.h"
#include <Windows.h>

TMyString::TMyString()
{
}


TMyString::~TMyString()
{
}

void TMyString::Split(TCHAR *src, std::vector<TCHAR *> &result, TCHAR *sub)
{
	int srclen = _tcslen(src);
	int sublen = _tcslen(sub);
	TCHAR *end = src + srclen;

	//得到所有位置
	TCHAR *tempstr = NULL;
	TCHAR *prev = src;
	TCHAR *now = NULL;
	while ((now = _tcsstr(prev, sub)) != NULL)
	{
		tempstr = new TCHAR[now - prev + 1];
		_tcsncpy(tempstr, prev, now - prev);
		tempstr[now - prev] = TEXT('\0');
		result.push_back(tempstr);
		prev = now + sublen;
	}
	tempstr = new TCHAR[end - prev + 1];
	_tcsncpy(tempstr, prev, end - prev);
	tempstr[end - prev] = TEXT('\0');
	result.push_back(tempstr);
}

void TMyString::Trim(TCHAR *str)
{
	TCHAR *start = str, *end = str+_tcslen(str)-1;
	while (*start == TEXT(' '))
	{
		start++;
	}
	while (*end == TEXT(' '))
	{
		end--;
	}
	_tcsncpy(str, start, end - start + 1);
	str[end - start+1] = TEXT('\0');
}

void TMyString::ReplaceLoop(TCHAR *src, TCHAR *sub, TCHAR *dest)
{
	while (1)
	{
		if (Replace(src, sub, dest) == 0)
			break;
	}
}

int TMyString::Replace(TCHAR *src, TCHAR *sub, TCHAR *dest)
{
	int srclen = _tcslen(src);
	if (srclen == 0) return 0;
	int sublen = _tcslen(sub);
	if (sublen == 0) return 0;
	int destlen = _tcslen(dest);
	std::vector<TCHAR *> pos;//保存寻找到的位置

	//得到所有位置
	TCHAR *temp = src;
	TCHAR *now = NULL;
	while ((now = _tcsstr(temp, sub)) != NULL)
	{
		pos.push_back(now);
		temp = now + sublen;
	}

	if (pos.size() == 0) return 0;
	int newsize = srclen + pos.size()*(destlen - sublen) + 1;//新串的大小，加上末尾的\0
	TCHAR *newsrc = (TCHAR *)malloc(newsize*sizeof(TCHAR));
	newsrc[0] = TEXT('\0');
	TCHAR *prevpos = src;
	for (UINT i = 0; i < pos.size(); i++)
	{
		_tcsncat(newsrc, prevpos, pos[i] - prevpos);
		_tcsncat(newsrc, dest, destlen);
		prevpos = pos[i] + sublen;
	}
	_tcsncat(newsrc, prevpos, srclen - (prevpos - src));
	//此处newsrc已全部拷完

	_tcscpy(src, newsrc);
	free(newsrc);
	return pos.size();
}

/* 字符是a-zA-z或_ */
bool TMyString::isAlphaCharOrUnderline(TCHAR c)
{
	if ((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && c <= TEXT('Z'))
		|| c == TEXT('_'))
		return true;
	else
		return false;
}

bool TMyString::isNumberChar(TCHAR c)
{
	if (c >= TEXT('0') && c <= TEXT('9'))
		return true;
	else
		return false;
}

bool TMyString::isVariableName(TCHAR *varname)
{
	if (isAlphaCharOrUnderline(*varname) == false)
		return false;
	while (*varname != TEXT('\0'))
	{
		if (isNumberChar(*varname) || isAlphaCharOrUnderline(*varname))
			varname++;
		else
			return false;
	}
	return true;
}