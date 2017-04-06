#pragma once
#include "DetectMemoryLeak.h"

#include "MyMath.h"
#include <time.h>

#include "TTransfer.h"

#include "TSolver.h"

#include "TMyString.h"

#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"
#include "TVariableTable.h"
#include "TEquations.h"
#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"

TSolver::TSolver()
{
	bOutput = false;
	hwndOutput = NULL;
	Equations = NULL;
	Str = TEXT("");
}


TSolver::~TSolver()
{
	ClearEuqations();
}

void TSolver::SetHwnd(HWND hwnd)
{
	hwndOutput = hwnd;
	if (hwnd != NULL)
		bOutput = true;
	else
		bOutput = false;
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
	i = TTransfer::TCHAR2int(temp);

	delete[] temp;
	return i;
}

void TSolver::ClearConstraint()
{
	Equations->RemoveTempEquations();
}

//添加鼠标约束
void TSolver::AddMouseConstraint(int index, DPOINT dpt)
{
	switch (pShape->Element[index]->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_SLIDER:
	{
		TCHAR temp[200];
		TElement *element = pShape->Element[index];
		double xm = dpt.x;
		double ym = dpt.y;
		int i = element->id;

		_stprintf(temp, TEXT("(x%d-%f)*sin(phi%d)-(y%d-%f)*cos(phi%d)"), i, xm, i, i, ym, i);

		TCHAR szVar[100], szValue[100];
		_stprintf(szVar, TEXT("x%d y%d phi%d"), i, i, i);
		_stprintf(szValue, TEXT("%f %f %f"), element->dpt.x, element->dpt.y, element->angle);

		Outputln(Equations->DefineVariable(bOutput, szVar, szValue));
		Outputln(Equations->AddEquation(bOutput, temp, true));

		return;
	}
	case ELEMENT_FRAMEPOINT:
	case ELEMENT_SLIDEWAY:
	case CONSTRAINT_COINCIDE:
		return;
	default:
		assert(0);
		return;
	}
}

void TSolver::ClearEuqations()
{
	if (Equations != NULL)
		delete Equations;
	Equations = new TEquations;
}

void TSolver::RefreshEquations()
{
	Str = TEXT("");
	_tcscpy(subsVar, TEXT(""));
	_tcscpy(subsValue, TEXT(""));
	ClearEuqations();

	Outputln(TEXT("自由度: DOF = nc - nh = nb*3 - nh = %d*3 - %d = %d"), pShape->nb,pShape->nh(), pShape->DOF());
	int i, j;
	DPOINT SiP, SjP,SiQ,SjQ;
	TCHAR buffer1[1024], buffer2[1024];

	Outputln(TEXT("\r\n约束方程:"));

	for (auto element : pShape->Element)
	{
		switch (element->eType)
		{
		case CONSTRAINT_COINCIDE:
		{
			pShape->GetSijP((TConstraintCoincide *)element, SiP, SjP, i, j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[0], &xi, &yi, &phii);
			pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[1], &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations->DefineVariable(bOutput, buffer1, buffer2);

			//读入方程
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d-%f*sin(phi%d)-%f*cos(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			Outputln(Equations->AddEquation(bOutput, buffer1, false));
			Outputln(Equations->AddEquation(bOutput, buffer2, false));
			break;
		}
		case CONSTRAINT_COLINEAR:
		{
			TConstraintColinear *pColinear = (TConstraintColinear *)element;
			pShape->GetSP(pColinear->pElement[0], 0, SiP, i);
			pShape->GetSQ(pColinear->pElement[0], 0, SiQ, i);

			pShape->GetSP(pColinear->pElement[1], 0, SjP, j);
			pShape->GetSQ(pColinear->pElement[1], 0, SjQ, j);

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(pColinear->pElement[0], &xi, &yi, &phii);
			pShape->GetCoordinateByElement(pColinear->pElement[1], &xj, &yj, &phij);

			//定义变量及其初始值
			_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			Equations->DefineVariable(bOutput, buffer1, buffer2);

			//读入方程
			_stprintf(buffer1, TEXT("(cos(phi%d)*(%f-%f)+sin(phi%d)*(%f-%f))*(x%d-x%d+%f*cos(phi%d)-%f*cos(phi%d)-%f*sin(phi%d)+%f*sin(phi%d))\
								-(cos(phi%d)*(%f-%f)-sin(phi%d)*(%f-%f))*(y%d-y%d+%f*cos(phi%d)-%f*cos(phi%d)+%f*sin(phi%d)-%f*sin(phi%d))"),
																	i, SiP.y, SiQ.y, i, SiP.x, SiQ.x, i, j, SiP.x, i, SjP.x, j, SiP.y, i, SjP.y, j,
																	i, SiP.x, SiQ.x, i, SiP.y, SiQ.y, i, j, SiP.y, i, SjP.y, j, SiP.x, i, SjP.x, j);
			_stprintf(buffer2, TEXT("(cos(phi%d-phi%d)*(%f-%f)+sin(phi%d-phi%d)*(%f-%f))*(%f-%f)-(cos(phi%d-phi%d)*(%f-%f)-sin(phi%d-phi%d)*(%f-%f))*(%f-%f)"),
										i,j,SiP.y,SiQ.y,i,j,SiP.x,SiQ.x,SjP.x,SjQ.x,i,j,SiP.x,SiQ.x,i,j,SiP.y,SiQ.y,SjP.y,SjQ.y);

			Outputln(Equations->AddEquation(bOutput, buffer1, false));
			Outputln(Equations->AddEquation(bOutput, buffer2, false));
			break;
		}
		case ELEMENT_SLIDEWAY:
		case ELEMENT_FRAMEPOINT:
		{
			int id = element->id;

			_stprintf(subsVar, TEXT("%s x%d y%d phi%d"), subsVar, id, id, id);
			_stprintf(subsValue, TEXT("%s %f %f %f"), subsValue, element->dpt.x,element->dpt.y, element->angle);
			//Solve时，建立Jacobian会替换掉
			break;
		}
		}
	}

	if (Equations->GetEquationsCount() == 0)
		Outputln(TEXT("\r\n无约束。\r\n"));

	if (pShape->DOF() == 1)
		Outputln(TEXT("\r\n可以拖动。"));
	else
	if (pShape->DOF() > 1)
		Outputln(TEXT("\r\n欠约束。"));

	if (bOutput)
		RefreshWindowText();
}

void TSolver::ClearOutput()
{
	Str = TEXT("");
}

//必须使用了本函数才刷新Edit内容
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

//求解
void TSolver::Solve()
{

	clock_t start, stop;//clock_t是clock()函数返回类型
	double duration;

	start = clock();

	Outputln(Equations->Subs(bOutput, subsVar, subsValue));
	Outputln(Equations->SimplifyEquations(bOutput));
	Outputln(TEXT("简化后变量："));
	Outputln(Equations->VariableTableUnsolved.Output());//输出当前变量
	Outputln(Equations->BuildJacobian(bOutput));//建立Jacobian, subsVar, subsValue
	Outputln(Equations->SolveEquations(bOutput));//解方程

	if (Equations->hasSolved)//解出
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
			case ELEMENT_SLIDEWAY:
			{
				TBar *bar = (TBar *)element;
				switch (eQType)
				{
				case x:
					((TBar *)bar)->dpt.x = data;
					break;
				case y:
					((TBar *)bar)->dpt.y = data;
					break;
				case phi:
					data = MakeIn2Pi(data);
					bar->SetPoint(bar->ptBegin, bar->dLength,data );
					break;
				}
				break;
			}
			case ELEMENT_FRAMEPOINT:
			case ELEMENT_SLIDER:
			{
				switch (eQType)
				{
				case x:
					element->dpt.x = data;
					break;
				case y:
					element->dpt.y = data;
					break;
				case phi:
					data = MakeIn2Pi(data);
					element->angle = data;
					break;
				}
				break;
			}
			default:
				assert(0);
				break;
			}
		}
	}

	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK;

	Outputln(TEXT("\r\n耗时 %f s"), duration);

	if (bOutput) RefreshWindowText();
}

void TSolver::Demo()
{
	//Outputln(Equations->VariableTable.Define(true, TEXT("x1 y1 phi1 x2 y2 phi2 l t"),TEXT("0 0 0 2.3 0 0 1.3 0")));
	//Outputln(Equations->AddEquation(true, TEXT("x1"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y1"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y2"), false));
	//Outputln(Equations->AddEquation(true, TEXT("x1-x2+cos(phi1)+l*sin(phi2)"), false));
	//Outputln(Equations->AddEquation(true, TEXT("y1-y2+sin(phi1)-l*cos(phi2)"), false));
	//Outputln(Equations->AddEquation(true, TEXT("phi1-t"), false));
	//Outputln(Equations->BuildJacobian(true, TEXT("l t"), TEXT("1.3 0")));

	//Outputln(Equations->VariableTable.Define(true, TEXT("x2 phi2"), TEXT("0 0")));
	Outputln(Equations->DefineVariable(true, TEXT("x2 phi2 z t l"), TEXT("0 0 0 0 0")));
	Outputln(Equations->AddEquation(true, TEXT("-x2+1+1.3*sin(phi2)*l"), false));
	Outputln(Equations->AddEquation(true, TEXT("-1.3*(-phi2/(5+4))-20"), false));
	Outputln(Equations->AddEquation(true, TEXT("2*sin(z)+0.5*cos(z)+t"), false));
	Outputln(Equations->AddEquation(true, TEXT("t-0.3"), false));
	Outputln(Equations->Subs(true, TEXT("l"), TEXT("1.3")));
	Outputln(Equations->SimplifyEquations(true));
	Outputln(Equations->BuildJacobian(true));
	Outputln(Equations->SolveEquations(true));
	RefreshWindowText();
	//建立ExpressionTree
	//TExpressionTree ex;
	//TVariableTable VarTable;
	//Outputln(VarTable.Define(true,TEXT("  x   y z   ")));
	//Outputln(ex.LinkVariableTable(&VarTable));
	//Outputln(ex.Read(TEXT("sqrt(x/y)"), true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Diff(TEXT("x"), 1, true));
	//Outputln(ex.Simplify(true));
	//Outputln(ex.Subs(TEXT("x y"), TEXT(" y 0.1 "), true));
	//Outputln(ex.Simplify(true));
	//Outputln(TEXT("%f"), ex.Value(true));
}