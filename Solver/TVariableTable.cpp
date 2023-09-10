#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "..\Common\DetectMemoryLeak.h"
#include "../Common/String.h"
#include "..\Solver\TVariableTable.h"

#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <sstream>

using namespace std;

TVariableTable::TVariableTable()
{
	bShared = false;
	eError = ERROR_NO;
}


TVariableTable::~TVariableTable()
{
}

double TVariableTable::GetValueFromVarPoint(const std::string VarStr)
{
	auto it = FindVariableTable(VarStr);
	if (it == VariableTable.end())
	{
		eError = ERROR_UNDEFINED_VARIABLE;
		throw TError{ eError, VarStr };
	}
	return VariableValue[it - VariableTable.begin()];
}

void TVariableTable::SetValueFromVarStr(std::string VarStr, double value)
{
	auto it = FindVariableTable(VarStr);
	if (it == VariableTable.end())
	{
		eError = ERROR_UNDEFINED_VARIABLE;
		throw TError{ eError, VarStr };
	}
	VariableValue[it - VariableTable.begin()] = value;
}

std::vector<std::string>::iterator TVariableTable::FindVariableTable(const std::string varstr)//查找变量是否在变量表中，没有则返回false
{
	return std::find(VariableTable.begin(), VariableTable.end(), varstr);
}

//连带value一起删
void TVariableTable::RemoveOne(std::ostream *pStr, std::string var, bool bIgnoreUnExisted)
{
	auto it = FindVariableTable(var);
	if (it == VariableTable.end())//未定义
	{
		if (bIgnoreUnExisted)
			return;
		else
		{
			eError = ERROR_UNDEFINED_VARIABLE;
			throw TError{ eError, var };
			return;
		}
	}
	int index = it - VariableTable.begin();
	VariableTable.erase(it);
	VariableValue.erase(VariableValue.begin() + index);
}

//void TVariableTable::Remove(std::ostream *pStr, const std::string vars, bool bIgnoreUnExisted)
//{
//	std::vector<std::string> temp = StrSliceToVector(vars);
//	for (auto &var : temp)
//	{
//		RemoveOne(pStr,var,bIgnoreUnExisted);
//	}
//}

void TVariableTable::DefineOne(std::ostream *pStr, std::string var, double value, bool bIgnoreReDef)
{
	if (!isLegalName(var))
	{
		eError = ERROR_INVALID_VARNAME;
		throw TError{ eError, var };
		return;
	}

	auto it = FindVariableTable(var);
	if (it != VariableTable.end())//已定义
	{
		if (bIgnoreReDef)
		{
			VariableValue[it - VariableTable.begin()] = value;
			return;
		}
		else
		{
			eError = ERROR_VAR_HAS_BEEN_DEFINED;
			throw TError{ eError, var };
			return;
		}
	}
	else
	{
		VariableTable.push_back(var);
		VariableValue.push_back(value);
	}

	if (pStr != NULL)
	{
		*pStr << TEXT(">>Define: ");

		*pStr << var << TEXT("(");
		*pStr << value << TEXT(") ");
		*pStr << TEXT("\r\n\r\n");
	}

}

//新定义的若与旧的重名可过滤掉重定义
void TVariableTable::Define(std::ostream *pStr, std::string input_str, std::string input_num, bool bIgnoreReDef)
{
	using namespace std;
	//切分str，new出每个新变量

	std::vector<std::string> tempVar = StrSliceToVector(input_str);
	std::vector<double> tempValue;
	if (input_num.empty())
		tempValue = std::vector<double>(tempVar.size());
	else
		tempValue = StrSliceToDoubleVector(input_num);

	if (tempVar.size() != tempValue.size())
	{
		if (pStr != NULL) *pStr << TEXT("变量名与初始值数量不对等。");
		eError = ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT;
		throw TError{ eError, "VAR:" + input_str + " VALUE:" + input_num };
		return;
	}

	for (int i = 0; i < tempVar.size(); ++i)
		DefineOne(pStr, tempVar[i], tempValue[i], bIgnoreReDef);

}

void TVariableTable::Output(std::ostream *pStr)
{
	if (pStr != NULL)
	{
		*pStr << TEXT("已定义变量(");
		*pStr << VariableTable.size();
		*pStr << TEXT("个):");

		for (auto &Var : VariableTable)
		{
			*pStr << TEXT(" ");
			*pStr << Var;
		}

		*pStr << TEXT("\r\n");
	}
}

//不删掉原变量，仅仅加上VarTable的值
void TVariableTable::SetValueByVarTable(TVariableTable &VarTable)
{
	for (int i = 0; i < VarTable.VariableTable.size(); ++i)
	{
		auto it = FindVariableTable(VarTable.VariableTable[i]);
		if (it != VariableTable.end())
		{
			VariableValue[it - VariableTable.begin()] = VarTable.VariableValue[i];
		}
	}
}

void TVariableTable::OutputValue(std::ostream *pStr)//输出 x=0 形式
{
	if (pStr != NULL)
	{
		for (size_t i = 0; i < VariableTable.size(); ++i)
		{
			*pStr << VariableTable[i];
			*pStr << TEXT(" = ");
			*pStr << VariableValue[i];
			*pStr << TEXT("\r\n");
		}
		*pStr << TEXT("\r\n");
	}
}


bool TVariableTable::isLegalName(std::string s)
{
	if (s.empty()) return false;
	if (s[0] == '_' || (s[0] >= 'A' && s[0] <= 'z'))
	{
		for (auto c : s)
			if (c == '_' || (c >= 'A'&& c <= 'z') || (c >= '0'&& c <= '9'))
				;
			else
			{
				return false;
			}
	}
	return true;
}

