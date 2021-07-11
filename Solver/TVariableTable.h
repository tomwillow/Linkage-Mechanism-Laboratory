#pragma once
#include <vector>
#include <map>

#include "..\Common\String.h"
#include "..\enumError.h"

class TVariableTable
{
private:
	bool TVariableTable::isLegalName(String s);
public:
	bool bShared;//����ǹ����������deleteԪ��
	enumError eError;
	std::vector<String> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	std::vector<String>::iterator TVariableTable::FindVariableTable(const String varstr);//���ұ����Ƿ��ڱ������У�û���򷵻�false
	void TVariableTable::Define(Ostream *pStr, String vars, String input_num = TEXT(""),bool bIgnoreReDef = false);
	void TVariableTable::DefineOne(Ostream *pStr,String var, double value,bool bIgnoreReDef=false);//���嵥����
	void TVariableTable::Output(Ostream *pStr);
	void TVariableTable::OutputValue(Ostream *pStr);//��� x=0 ��ʽ
	//void TVariableTable::Remove(Ostream *pStr, const String vars, bool bIgnoreUnExisted = false);
	void TVariableTable::RemoveOne(Ostream *pStr, String var, bool bIgnoreUnExisted = false);//ͬʱ�������������
	double TVariableTable::GetValueFromVarPoint(const String pVar);
	void TVariableTable::SetValueFromVarStr(String VarStr, double value);
	void TVariableTable::SetValueByVarTable(TVariableTable &VarTable);
};

