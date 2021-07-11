#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "..\Common\DetectMemoryLeak.h"
#include "..\Common\String.h"
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

double TVariableTable::GetValueFromVarPoint(const String VarStr)
{
	auto it = FindVariableTable(VarStr);
	if (it == VariableTable.end())
	{
		eError = ERROR_UNDEFINED_VARIABLE;
		throw TError{ eError, VarStr };
		return 0.0;
	}
	return VariableValue[it - VariableTable.begin()];
}

void TVariableTable::SetValueFromVarStr(String VarStr, double value)
{
	auto it = FindVariableTable(VarStr);
	if (it == VariableTable.end())
	{
		eError = ERROR_UNDEFINED_VARIABLE;
		throw TError{ eError, VarStr };
		return;
	}
	VariableValue[it - VariableTable.begin()] = value;
}

std::vector<String>::iterator TVariableTable::FindVariableTable(const String varstr)//���ұ����Ƿ��ڱ������У�û���򷵻�false
{
	return std::find(VariableTable.begin(), VariableTable.end(), varstr);
}

//����valueһ��ɾ
void TVariableTable::RemoveOne(Ostream *pStr, String var, bool bIgnoreUnExisted)
{
	auto it = FindVariableTable(var);
	if (it == VariableTable.end())//δ����
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

//void TVariableTable::Remove(Ostream *pStr, const String vars, bool bIgnoreUnExisted)
//{
//	std::vector<String> temp = StrSliceToVector(vars);
//	for (auto &var : temp)
//	{
//		RemoveOne(pStr,var,bIgnoreUnExisted);
//	}
//}

void TVariableTable::DefineOne(Ostream *pStr, String var, double value, bool bIgnoreReDef)
{
	if (!isLegalName(var))
	{
		eError = ERROR_INVALID_VARNAME;
		throw TError{ eError, var };
		return;
	}

	auto it = FindVariableTable(var);
	if (it != VariableTable.end())//�Ѷ���
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

//�¶��������ɵ������ɹ��˵��ض���
void TVariableTable::Define(Ostream *pStr, String input_str, String input_num, bool bIgnoreReDef)
{
	using namespace std;
	//�з�str��new��ÿ���±���

	std::vector<String> tempVar = StrSliceToVector(input_str);
	std::vector<double> tempValue;
	if (input_num.empty())
		tempValue = std::vector<double>(tempVar.size());
	else
		tempValue = StrSliceToDoubleVector(input_num);

	if (tempVar.size() != tempValue.size())
	{
		if (pStr != NULL) *pStr << TEXT("���������ʼֵ�������Եȡ�");
		eError = ERROR_VAR_COUNT_NOT_EQUAL_NUM_COUNT;
		throw TError{ eError, TEXT("VAR:") + input_str + TEXT(" VALUE:") + input_num };
		return;
	}

	for (int i = 0; i < tempVar.size(); ++i)
		DefineOne(pStr, tempVar[i], tempValue[i], bIgnoreReDef);

}

void TVariableTable::Output(Ostream *pStr)
{
	if (pStr != NULL)
	{
		*pStr << TEXT("�Ѷ������(");
		*pStr << VariableTable.size();
		*pStr << TEXT("��):");

		for (auto &Var : VariableTable)
		{
			*pStr << TEXT(" ");
			*pStr << Var;
		}

		*pStr << TEXT("\r\n");
	}
}

//��ɾ��ԭ��������������VarTable��ֵ
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

void TVariableTable::OutputValue(Ostream *pStr)//��� x=0 ��ʽ
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


bool TVariableTable::isLegalName(String s)
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

