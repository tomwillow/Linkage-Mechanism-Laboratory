#pragma once
#include <tchar.h>
#include <vector>

class TVariableTable
{
private:
	int input_len;
	TCHAR *info;
	void TVariableTable::ReleaseVariableTable(std::vector<TCHAR *> &input);
	void TVariableTable::DeleteByAddress(TCHAR *var);
public:
	std::vector<TCHAR *> VariableTable;
	std::vector<double> VariableValue;
	TVariableTable();
	~TVariableTable();
	TCHAR * TVariableTable::FindVariableTable(TCHAR *varstr);//查找变量是否在变量表中，没有则返回NULL
	TCHAR * TVariableTable::Define(TCHAR *input,TCHAR *input_num=NULL);
	TCHAR * TVariableTable::Output();
	TCHAR * TVariableTable::Remove(TCHAR *input);
};

