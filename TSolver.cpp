#include "TSolver.h"

#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"
#include "TVariableTable.h"
#include "TEquations.h"

TSolver::TSolver()
{
	bOutput = false;
	hwndOutput = NULL;
	//str = NULL;
	str = (TCHAR *)malloc(1 * sizeof(TCHAR));
	str[0] = TEXT('\0');
}


TSolver::~TSolver()
{
	if (str != NULL)
		free(str);

	for (int i = 0; i < InputConstraintStr.size(); i++)
		delete[] InputConstraintStr[i];
}

void TSolver::SetHwnd(HWND hwnd)
{
	hwndOutput = hwnd;
}

void TSolver::Outputln(const TCHAR *szFormat, ...)
{
	if (bOutput == false)
		return;
	if (szFormat != NULL && _tcslen(szFormat) > 0)
	{
		TCHAR *szBuffer = NULL;
		szBuffer = new TCHAR[_tcslen(szFormat) + 1024];
		//TCHAR szBuffer[102400];
		va_list pArgList;
		va_start(pArgList, szFormat);
		_vsntprintf(szBuffer, _tcslen(szFormat) + 1024, szFormat, pArgList);
		va_end(pArgList);

		//追加换行
		int str_num = 0;
		if (str == NULL)
			str_num = 0;
		else
			str_num = _tcslen(str);
		int buffer_num = _tcslen(szBuffer);
		str = (TCHAR *)realloc(str, (str_num + buffer_num + 3)*sizeof(TCHAR));
		_tcscat(str, szBuffer);
		_tcscat(str, TEXT("\r\n"));

		delete[] szBuffer;
		if (hwndOutput) SetWindowText(hwndOutput, str);
	}
}

void TSolver::Output(TCHAR *szFormat, ...)
{
	if (szFormat == NULL || bOutput==false)
		return;
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	int str_num = 0;
	if (str == NULL)
		str_num = 0;
	else
		str_num = _tcslen(str);
	int buffer_num = _tcslen(szBuffer);
	str = (TCHAR *)realloc(str, (str_num + buffer_num + 1)*sizeof(TCHAR));
	_tcscat(str, szBuffer);

	if (hwndOutput) SetWindowText(hwndOutput, str);
}

int TSolver::GetIdFromVariableStr(TCHAR varname[])
{
	int i = -1;
	TCHAR *temp = new TCHAR[10];
	int len = _tcslen(varname);
	TCHAR *end = varname + len;
	while (*varname != TEXT('\0'))
	{
		if (*varname >= TEXT('0') && *varname <= TEXT('9'))
		{
			break;
		}
		varname++;
	}
	_tcsncpy(temp, varname, end - varname + 1);
	i = _wtoi(temp);

	delete[] temp;
	return i;
}

void TSolver::AddMouseConstraint(int index, DPOINT dpt)
{
	if (pShape->Element[index]->eType == ELEMENT_BAR)
	{
		TCHAR *temp = new TCHAR[200];
		TBar *bar = (TBar *)pShape->Element[index];
		double xi = bar->ptBegin.x;
		double yi = bar->ptBegin.y;
		double l = bar->dLength;
		double xm = dpt.x;
		double ym = dpt.y;
		int id = bar->id;

		_stprintf(temp, TEXT("(%f-%f)*(%f-%f*sin(phi%d))-(%f-%f*cos(phi%d))*(%f-%f)"), xi, xm, yi, l, id, xi, l, id, yi, ym);
		//TEXT("10*-48*sin(phi1)+48*cos(phi1)*-10")
		InputConstraintStr.push_back(temp);
	}
}

void TSolver::Solute(bool Output)
{
	bOutput = Output;
	Outputln(TEXT("DOF=nc-nh: %d=%d-%d"), pShape->DOF(), pShape->nc(), pShape->nh());
	int i, j;
	DPOINT SiP, SjP;
	TCHAR buffer1[100], buffer2[100];

	TEquations Equations;

	TCHAR subsVar[1000] = { TEXT("\0") }, subsValue[1000] = { TEXT("\0") };

	//TEquations Eq;
	//Outputln(Eq.VariableTable.Define(TEXT("phi9 x11 y11 x13 y13 phi13"), TEXT("")));
	//Outputln(Eq.AddEquation(TEXT("x11 - 40*cos(phi9)"), true));
	//Outputln(Eq.AddEquation(TEXT("y11 + 40*sin(phi9)"), true));
	//Outputln(Eq.AddEquation(TEXT("x13 - x11 - 80*cos(0.5)"), true));
	//Outputln(Eq.AddEquation(TEXT("y13 - y11 + 80*sin(0.5)"), true));
	//Outputln(Eq.AddEquation(TEXT("x13 + 40*cos(phi13) - 160"), true));
	//Outputln(Eq.AddEquation(TEXT("y13 + 40*sin(phi13)"), true));
	//Outputln(Eq.BuildJacobi(true, NULL, NULL));
	//Outputln(Eq.Solve(true));

	//TEquations Eq;
	//Outputln(Eq.VariableTable.Define(TEXT("x"), TEXT("1.5")));
	//Outputln(Eq.AddEquation(TEXT("ln(x+sin(x))"),true));
	//Outputln(Eq.BuildJacobi(true,NULL, NULL));
	//Outputln(Eq.Solve(true));

	Outputln(TEXT("生成方程:"));

	for (unsigned int index = 0; index < pShape->Element.size(); index++)
	{
		if (pShape->Element[index]->eType == CONSTRAINT_COINCIDE)
		{
			pShape->GetSijP(index, &SiP, &SjP, &i, &j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinate(i, &xi, &yi, &phii);
			pShape->GetCoordinate(j, &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations.VariableTable.Define(buffer1, buffer2);

			//读入方程
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d+%f*sin(phi%d)-%f*cos(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			Outputln(Equations.AddEquation(buffer1, Output));
			Outputln(Equations.AddEquation(buffer2, Output));

		}
		if (pShape->Element[index]->eType == ELEMENT_FRAMEPOINT)
		{
			int id = pShape->Element[index]->id;

			_stprintf(subsVar, TEXT("%s x%d y%d phi%d"), subsVar, id, id, id);

			_stprintf(subsValue, TEXT("%s %f %f %f"), subsValue, ((TFramePoint *)(pShape->Element[index]))->dpt.x, ((TFramePoint *)(pShape->Element[index]))->dpt.y, 0.0);

		}
	}

	for (int i = 0; i < InputConstraintStr.size(); i++)
		Outputln(Equations.AddEquation(InputConstraintStr[i], Output));//phi9约40度phi11-0.5(40-x11)^2+(30-y11)^2-40^2

	Outputln(Equations.BuildJacobi(Output, subsVar, subsValue));
	Outputln(Equations.VariableTable.Output());
	Outputln(Equations.Solve(Output));

	for (int i = 0; i < Equations.VariableTable.VariableTable.size(); i++)
	{
		enum enumQType{ x, y, phi } eQType;
		switch (Equations.VariableTable.VariableTable[i][0])
		{
		case TEXT('x'):
			eQType = x;
			break;
		case TEXT('y'):
			eQType = y;
			break;
		case TEXT('p'):
			eQType = phi;
			break;
		}
		int id = GetIdFromVariableStr(Equations.VariableTable.VariableTable[i]);
		TElement *element = pShape->GetElementById(id);
		double data = Equations.VariableTable.VariableValue[i];

		switch (element->eType)
		{
		case ELEMENT_BAR:
		{
			TBar *bar = (TBar *)element;
			switch (eQType)
			{
			case x:
				((TBar *)bar)->ptBegin.x = data;
				break;
			case y:
				((TBar *)bar)->ptBegin.y = data;
				break;
			case phi:
				//bar->dAngle = data;
				bar->SetPoint(bar->ptBegin, bar->dLength, data);
				break;
			}
			break;
		}
		}
	}


	//TExpressionTree ex;
	//TVariableTable VarTable;
	//Outputln(VarTable.Define(TEXT("  x   y z   ")));
	//Outputln(ex.LinkVariableTable(&VarTable));
	//Outputln(ex.Read(TEXT("sqrt(x/y)"), true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Diff(TEXT("x"), 1, true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Subs(TEXT("x y"), TEXT(" y 0.1 "), true));
	//Outputln(ex.Simplify(true));
	//Outputln(TEXT("%f"), ex.Value(true));

}