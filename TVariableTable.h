#pragma once
#include "tchar_head.h"
#include <vector>
#include <map>

#include "String.h"
#include "enumError.h"

class TVariableTable
{
private:
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
public:
	bool bShared;//如果是共享变量表则不delete元素
	enumError eError;
	//std::map<TCHAR *, double> VariableTable;
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	void TVariableTable::DeleteByAddress(TCHAR *var);//同时清除变量和数据
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//查找变量是否在变量表中，没有则返回NULL
	void TVariableTable::Define(String *pStr, TCHAR *input_str, TCHAR *input_num = NULL);
	void TVariableTable::Define(String *pStr, TCHAR *input_str, double value);
	void TVariableTable::Output(String *pStr);
	void TVariableTable::OutputValue(String *pStr);//输出 x=0 形式
	void TVariableTable::Remove(String *pStr, const TCHAR input_str[]);
	double TVariableTable::GetValueFromVarPoint(TCHAR *pVar);
	void TVariableTable::SetValueFromVarStr(TCHAR *VarStr, double value);
	void TVariableTable::SetValueByVarTable(TVariableTable &VarTable);
};

