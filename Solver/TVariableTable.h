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
	bool bShared;//����ǹ����������deleteԪ��
	enumError eError;
	std::vector<String> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	std::vector<String>::iterator FindVariableTable(const String varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�false
	void Define(Ostream *pStr, String vars, String input_num = TEXT(""),bool bIgnoreReDef = false);
	void DefineOne(Ostream *pStr,String var, double value,bool bIgnoreReDef=false);//���嵥����
	void Output(Ostream *pStr);
	void OutputValue(Ostream *pStr);//��� x=0 ��ʽ
	//void Remove(Ostream *pStr, const String vars, bool bIgnoreUnExisted = false);
	void RemoveOne(Ostream *pStr, String var, bool bIgnoreUnExisted = false);//ͬʱ�������������
	double GetValueFromVarPoint(const String pVar);
	void SetValueFromVarStr(String VarStr, double value);
	void SetValueByVarTable(TVariableTable &VarTable);
};

