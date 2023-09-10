#pragma once
#include <vector>
#include <map>

#include "..\enumError.h"

class TVariableTable
{
private:
	bool isLegalName(std::string s);
public:
	bool bShared;//如果是共享变量表则不delete元素
	enumError eError;
	std::vector<std::string> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	std::vector<std::string>::iterator FindVariableTable(const std::string varstr);//查找变量是否在变量表中，没有则返回false
	void Define(std::ostream *pStr, std::string vars, std::string input_num = "",bool bIgnoreReDef = false);
	void DefineOne(std::ostream *pStr,std::string var, double value,bool bIgnoreReDef=false);//定义单变量
	void Output(std::ostream *pStr);
	void OutputValue(std::ostream *pStr);//输出 x=0 形式
	//void Remove(std::ostream *pStr, const std::string vars, bool bIgnoreUnExisted = false);
	void RemoveOne(std::ostream *pStr, std::string var, bool bIgnoreUnExisted = false);//同时清除变量和数据
	double GetValueFromVarPoint(const std::string pVar);
	void SetValueFromVarStr(std::string VarStr, double value);
	void SetValueByVarTable(TVariableTable &VarTable);
};

