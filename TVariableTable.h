#pragma once
#include <tchar.h>
#include <vector>

class TVariableTable
{
public:
	std::vector<TCHAR *> VariableTable;
	TVariableTable();
	~TVariableTable();
};

