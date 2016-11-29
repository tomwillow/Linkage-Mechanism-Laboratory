#pragma once
#include <vector>
#include "TTool.h"

#include "DPOINT.h"

enum enumConditionType {CONDITION_COINCIDE,CONDITION_FRAMEPOINT};
class TSolve:public TTool
{
private:
	TCHAR *str;
	std::vector<int> idOrder;
	double *Phi, *Phip;
	struct TCondition
	{
		enumConditionType eType;
		int i, j;
		DPOINT SiP, SjP;
	};
	void TSolve::Output(TCHAR szFormat[], ...);
	HWND hwndOutput;
public:
	TSolve();
	~TSolve();
	void Solute();
	void TSolve::SetHwnd(HWND hwnd);
};

