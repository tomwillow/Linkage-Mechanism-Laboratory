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
	String str;
	//std::map<TCHAR *, double> VariableTable;
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	void TVariableTable::DeleteByAddress(TCHAR *var);//同时清除变量和数据
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//查找变量是否在变量表中，没有则返回NULL
	const TCHAR * TVariableTable::Define(bool bOutput, TCHAR *input, TCHAR *input_num = NULL);
	const TCHAR * TVariableTable::Output();
	void TVariableTable::OutputValue(String &s);//输出 x=0 形式
	const TCHAR * TVariableTable::Remove(const TCHAR input[]);
	double TVariableTable::GetValueFromVarPoint(TCHAR *pVar);
	void TVariableTable::SetValueFromVarStr(TCHAR *VarStr, double value);
	void TVariableTable::SetValueByVarTable(TVariableTable &VarTable);
};

