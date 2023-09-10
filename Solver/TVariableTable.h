#pragma once
#include <vector>
#include <map>

#include "..\Common\String.h"
#include "..\enumError.h"

class TVariableTable
{
private:
	bool isLegalName(String s);
public:
	bool bShared;//如果是共享变量表则不delete元素
	enumError eError;
	std::vector<String> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	std::vector<String>::iterator FindVariableTable(const String varstr);//查找变量是否在变量表中，没有则返回false
	void Define(Ostream *pStr, String vars, String input_num = TEXT(""),bool bIgnoreReDef = false);
	void DefineOne(Ostream *pStr,String var, double value,bool bIgnoreReDef=false);//定义单变量
	void Output(Ostream *pStr);
	void OutputValue(Ostream *pStr);//输出 x=0 形式
	//void Remove(Ostream *pStr, const String vars, bool bIgnoreUnExisted = false);
	void RemoveOne(Ostream *pStr, String var, bool bIgnoreUnExisted = false);//同时清除变量和数据
	double GetValueFromVarPoint(const String pVar);
	void SetValueFromVarStr(String VarStr, double value);
	void SetValueByVarTable(TVariableTable &VarTable);
};

