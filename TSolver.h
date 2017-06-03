#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "TTool.h"
#include "TVariableTable.h"

#include "String.h"
#include "DPOINT.h"

#include "DialogAnimation.h"

enum enumConditionType {CONDITION_COINCIDE,CONDITION_FRAMEPOINT};
class TEquations;
class TListBoxItem;
class TSolver:public TTool
{
private:
	String *pStr;
	std::vector<String> vecStrDriver;
	HWND hwndOutput;
	//TCHAR *str;
	TEquations *Equations;
	//TEquations *EquationsV;
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
	void TSolver::SetElementPosition(TVariableTable &VariableTable);
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
	bool TSolver::Solve(double t);
	void TSolver::LinkpValue(std::vector<double *> &vecpValue);
	void TSolver::GetResult(std::vector<double> &vecResult);
	void TSolver::SubsFramePoint();

	void TSolver::GetMesureResult(std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex);
	void TSolver::LinkMesureResult(const std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex);
};

