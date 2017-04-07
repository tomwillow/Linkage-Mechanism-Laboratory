#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "TTool.h"

#include "String.h"
#include "DPOINT.h"

enum enumConditionType {CONDITION_COINCIDE,CONDITION_FRAMEPOINT};
class TEquations;
class TSolver:public TTool
{
private:
	String *pStr;
	HWND hwndOutput;
	//TCHAR *str;
	TEquations *Equations;
	TCHAR subsVar[1000], subsValue[1000];
	std::vector<int> idOrder;
	double *Phi, *Phip;
	struct TCondition
	{
		enumConditionType eType;
		int i, j;
		DPOINT SiP, SjP;
	};
	int TSolver::GetIdFromVariableStr(TCHAR varname[]);
	void TSolver::Output(TCHAR szFormat[], ...);
	void TSolver::Outputln(const TCHAR *szFormat, ...);
public:
	TSolver();
	~TSolver();
	void TSolver::Solve();
	void TSolver::ClearOutput();
	void TSolver::RefreshEquations();
	void TSolver::AddMouseConstraint(int index, DPOINT dpt);
	void TSolver::SetHwnd(HWND hwnd);
	void TSolver::RefreshWindowText();
	void TSolver::ClearConstraint();
	void TSolver::Demo();
	void TSolver::ClearEuqations();
};

