#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include "DetectMemoryLeak.h"

#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "TSolver.h"

#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"
#include "TVariableTable.h"
#include "TEquations.h"
#include "TConstraintCoincide.h"

TSolver::TSolver()
{
	bOutput = false;
	hwndOutput = NULL;
	Equations = NULL;
	Str = TEXT("");
}


TSolver::~TSolver()
{
	delete Equations;
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
		TCHAR *szBuffer = new TCHAR[_tcslen(szFormat) + 1024];
		va_list pArgList;
		va_start(pArgList, szFormat);
		_vsntprintf(szBuffer, _tcslen(szFormat) + 1024, szFormat, pArgList);
		va_end(pArgList);

		//追加换行
		Str += szBuffer;
		Str+=TEXT("\r\n");

		delete[] szBuffer;
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

	Str += szBuffer;
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

void TSolver::ClearConstraint()
{
	Equations->RemoveTempEquations();
}

void TSolver::AddMouseConstraint(bool bOutput,int index, DPOINT dpt)
{
	if (pShape->Element[index]->eType == ELEMENT_BAR || pShape->Element[index]->eType == ELEMENT_REALLINE)
	{
		TCHAR temp[200];
		TElement *element = pShape->Element[index];
		double xm = dpt.x;
		double ym = dpt.y;
		int i = element->id;

		_stprintf(temp, TEXT("(x%d-%f)*sin(phi%d)-(y%d-%f)*cos(phi%d)"),  i,xm, i,  i,ym, i);

		TCHAR szVar[100], szValue[100];
		_stprintf(szVar, TEXT("x%d y%d phi%d"), i, i, i);
		_stprintf(szValue, TEXT("%f %f %f"), element->dpt.x, element->dpt.y, element->angle);

		Outputln(Equations->VariableTable.Define(true, szVar, szValue));
		Outputln(Equations->AddEquation(bOutput,temp,true));

	}
}

void TSolver::RefreshEquations(bool Output)
{
	bOutput = Output;
	Str = TEXT("");
	_tcscpy(subsVar, TEXT(""));
	_tcscpy(subsValue, TEXT(""));
	if (Equations != NULL)
		delete Equations;
	Equations = new TEquations;

	Outputln(TEXT("自由度: DOF = nc-nh = %d - %d = %d"), pShape->nc(), pShape->nh(), pShape->DOF());
	int i, j;
	DPOINT SiP, SjP;
	TCHAR buffer1[100], buffer2[100];

	Outputln(TEXT("\r\n约束方程:"));

	for (auto element : pShape->Element)
	{
		if (element->eType == CONSTRAINT_COINCIDE)
		{
			pShape->GetSijP(element, &SiP, &SjP, &i, &j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement1, &xi, &yi, &phii);
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement2, &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations->VariableTable.Define(Output,buffer1, buffer2);

			//读入方程
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d-%f*sin(phi%d)-%f*cos(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			Outputln(Equations->AddEquation(Output, buffer1, false));
			Outputln(Equations->AddEquation(Output,buffer2, false));

		}
		if (element->eType == ELEMENT_FRAMEPOINT)
		{
			int id = element->id;

			_stprintf(subsVar, TEXT("%s x%d y%d phi%d"), subsVar, id, id, id);

			_stprintf(subsValue, TEXT("%s %f %f %f"), subsValue, ((TFramePoint *)(element))->dpt.x, ((TFramePoint *)(element))->dpt.y, 0.0);

		}
	}

	if (Equations->GetEquationsCount() == 0)
		Outputln(TEXT("\r\n无约束。\r\n"));

	if (pShape->DOF() == 1)
		Outputln(TEXT("\r\n可以拖动。"));
	else
	if (pShape->DOF() > 1)
		Outputln(TEXT("\r\n欠约束。"));

	if (Output)
		RefreshWindowText();
}

void TSolver::ClearOutput()
{
	Str = TEXT("");
}

void TSolver::RefreshWindowText()
{
	if (hwndOutput != NULL)
	{
		SetWindowText(hwndOutput, Str.c_str());

		//SetFocus(Edit.m_hWnd);

		int len = ::GetWindowTextLength(hwndOutput);
		::PostMessage(hwndOutput, EM_SETSEL, len, len);
		PostMessage(hwndOutput, EM_SCROLLCARET, 0, 0);
	}
}

void TSolver::Solve(bool Output)
{
	bOutput = Output;

	clock_t start, stop;//clock_t是clock()函数返回类型
	double duration;

	start = clock();

	Outputln(Equations->BuildJacobi(Output, subsVar, subsValue));
	Outputln(Equations->VariableTable.Output());
	Outputln(Equations->SolveEquations(Output));

	if (Equations->hasSolved)
	{
		for (size_t i = 0; i < Equations->VariableTable.VariableTable.size(); i++)
		{
			enum enumQType{ x, y, phi } eQType;
			switch (Equations->VariableTable.VariableTable[i][0])
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
			int id = GetIdFromVariableStr(Equations->VariableTable.VariableTable[i]);
			TElement *element = pShape->GetElementById(id);
			double &data = Equations->VariableTable.VariableValue[i];

			switch (element->eType)
			{
			case ELEMENT_BAR:
			case ELEMENT_REALLINE:
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
					if (abs(data) > 2 * M_PI)
						data = abs(data / M_PI / 2 - (int)(data / M_PI / 2)) * 2 * M_PI;
					bar->SetPoint(bar->ptBegin, bar->dLength, data);
					break;
				}
				break;
			}
			}
		}
	}

	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK;

	Outputln(TEXT("\r\n耗时 %f ms"), duration);

	if (Output) RefreshWindowText();
}

void TSolver::Demo()
{
	TExpressionTree ex;
	TVariableTable VarTable;
	Outputln(VarTable.Define(true,TEXT("  x   y z   ")));
	Outputln(ex.LinkVariableTable(&VarTable));
	Outputln(ex.Read(TEXT("sqrt(x/y)"), true));
	Outputln(ex.Simplify(true));
	Outputln(ex.Diff(TEXT("x"), 1, true));
	Outputln(ex.Simplify(true));
	Outputln(ex.Subs(TEXT("x y"), TEXT(" y 0.1 "), true));
	Outputln(ex.Simplify(true));
	Outputln(TEXT("%f"), ex.Value(true));
}