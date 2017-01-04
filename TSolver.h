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
	std::String Str;
	bool bOutput;
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
	void TSolver::Solve(bool Output = false);
	void TSolver::ClearOutput();
	void TSolver::RefreshEquations(bool Output = false);
	void TSolver::AddMouseConstraint(bool bOutput,int index, DPOINT dpt);
	void TSolver::SetHwnd(HWND hwnd);
	void TSolver::RefreshWindowText();
	void TSolver::ClearConstraint();
	void TSolver::Demo();
};

