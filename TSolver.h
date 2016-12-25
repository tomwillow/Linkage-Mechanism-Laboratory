#pragma once
#include <vector>
#include <iostream>
#include "TTool.h"

#include "DPOINT.h"

enum enumConditionType {CONDITION_COINCIDE,CONDITION_FRAMEPOINT};
class TSolver:public TTool
{
private:
	bool bOutput;
	HWND hwndOutput;
	TCHAR *str;
	std::vector<int> idOrder;
	std::vector<TCHAR *> InputConstraintStr;
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
	void Solute(bool Output=false);
	void TSolver::AddMouseConstraint(int index, DPOINT dpt);
	void TSolver::SetHwnd(HWND hwnd);
};

