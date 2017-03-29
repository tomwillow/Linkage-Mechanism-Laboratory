#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "DetectMemoryLeak.h"

#include <iostream>
#include <Windows.h>
#include "TVariableTable.h"
#include "TMyString.h"
#include "TTransfer.h"

TVariableTable::TVariableTable()
{
	eError = ERROR_NO;
}


TVariableTable::~TVariableTable()
{
	ReleaseVariableTable(VariableTable);
}

void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input)
{
	for (auto szStr: input)
	{
		delete[] szStr;
	}
	input.clear();
}

double TVariableTable::GetValueFromVarPoint(TCHAR *pVar)
{
	for (size_t i = 0; i < VariableTable.size(); i++)
		if (pVar == VariableTable[i])
			return VariableValue[i];

	eError = ERROR_UNDEFINED_VARIABLE;
	throw eError;
}

TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr)
{
	for (auto szVar:VariableTable)
		if (_tcscmp(varstr, szVar) == 0)
			return szVar;
	return NULL;
}

//连带value一起删
void TVariableTable::DeleteByAddress(TCHAR *var)
{
	for (size_t i = 0; i < VariableTable.size(); i++)
	{
		if (VariableTable[i] == var)
		{
			delete[] VariableTable[i];
			std::vector<TCHAR *>::iterator iter = VariableTable.begin() + i;
			VariableTable.erase(iter);

			std::vector<double>::iterator iter2 = VariableValue.begin() + i;
			VariableValue.erase(iter2);
			break;
		}
	}
}

const TCHAR * TVariableTable::Remove(const TCHAR input_str[])
{
	TCHAR *input = new TCHAR[_tcslen(input_str) + 1];
	_tcscpy(input, input_str);

	std::vector<TCHAR *> temp;
	TMyString::Trim(input);
	TMyString::ReplaceLoop(input, TEXT("  "), TEXT(" "));
	TMyString::Split(input, temp, TEXT(" "));
	for (auto szInputVar: temp)
	{
		if (TMyString::isVariableName(szInputVar) == false)
		{
			//出现不合法的变量名
			str = TEXT("不合法的变量");
			str += szInputVar;
			str += TEXT("。");
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			delete[] input;
			return str.c_str();
		}
	}

	//找出存在的变量
	TCHAR *var;
	for (size_t i = 0; i < temp.size(); i++)
		if (var=FindVariableTable(temp[i]))
		{
			DeleteByAddress(var);
		}

	ReleaseVariableTable(temp);
	delete[] input;
	return Output();
}

//新定义的若与旧的重名可过滤掉重定义
const TCHAR * TVariableTable::Define(bool bOutput,TCHAR *input_str,TCHAR *input_num)
{
	//复制输入串
	TCHAR *input = new TCHAR[_tcslen(input_str) + 1];
	_tcscpy(input, input_str);

	//切分str，new出每个新变量
	std::vector<TCHAR *> temp;
	TMyString::Trim(input);
	TMyString::ReplaceLoop(input, TEXT("  "), TEXT(" "));
	TMyString::Split(input, temp, TEXT(" "));
	delete[] input;

	std::vector<double> vectorNums;
	if (input_num != NULL)
	{
		TCHAR *str_num = new TCHAR[_tcslen(input_num) + 1];
		_tcscpy(str_num, input_num);

		//切分str，new出每个新变量
		std::vector<TCHAR *> vectorStrNums;
		TMyString::Trim(str_num);
		TMyString::ReplaceLoop(str_num, TEXT("  "), TEXT(" "));
		TMyString::Split(str_num, vectorStrNums, TEXT(" "));
		delete[] str_num;

		//变量名与初始值数量不对等
		if (temp.size() != vectorStrNums.size())
		{
			str = TEXT("变量名与初始值数量不对等。");
			eError = ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT;
			ReleaseVariableTable(vectorStrNums);
			ReleaseVariableTable(temp);
			return str.c_str();

		}

		for (size_t i = 0; i < vectorStrNums.size(); i++)
		{
			vectorNums.push_back(TTransfer::TCHAR2double(vectorStrNums[i]));//不合法数值被转化为0
			delete[] vectorStrNums[i];
		}
	}

	//滤掉不合法输入
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (TMyString::isVariableName(temp[i])==false)
		{
			//出现不合法的变量名
			str = TEXT("不合法的变量");
			str += temp[i];
			str += TEXT("。");
			eError = ERROR_INVALID_VARNAME;
			ReleaseVariableTable(temp);
			return str.c_str();
		}
	}

	//
	for (size_t i = 0; i < temp.size(); i++)
		if (FindVariableTable(temp[i]))//已有的不再定义
		{
			delete[] temp[i];
		}
		else
		{//未定义的进行定义
			VariableTable.push_back(temp[i]);
			if (input_num == NULL)
				VariableValue.push_back(0.0);
			else
			VariableValue.push_back(vectorNums[i]);
		}

	if (bOutput)
		return Output();//输出变量情况
	else
	{
		str = TEXT("");
		return str.c_str();
	}
}

const TCHAR * TVariableTable::Output()
{
	str = TEXT("已定义变量(");

	TCHAR tc[8];
	_itow(VariableTable.size(), tc, 10);
	str += tc;

	str += TEXT("个):");
	for (auto Var: VariableTable)
	{
		str += TEXT(" ");
		str+=Var;
	}

	str += TEXT("\r\n");

	return str.c_str();
}