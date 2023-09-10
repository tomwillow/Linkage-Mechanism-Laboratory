#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include "..\Common\MyMath.h"
#include <time.h>

#include "..\Common\TTransfer.h"

#include "..\Solver\TSolver.h"

#include "..\Common\TCHAR_Function.h"

#include "..\Draw\TDraw.h"

#include "..\Element\TBar.h"
#include "..\Common\DPOINT.h"
#include "..\Control\TMainWindow.h"
#include "..\Element\TShape.h"
#include "..\Solver\TExpressionTree.h"
#include "..\Solver\TVariableTable.h"
#include "TEquations.h"
#include "..\Element\TConstraintCoincide.h"
#include "..\Element\TConstraintColinear.h"
#include "..\Element\TDriver.h"
#include "..\Element\TElement.h"

#include "..\Dialog\DialogAnimation.h"

#include <iomanip>
#include <locale>

TSolver::TSolver()
{
	hwndOutput = NULL;
	Equations = NULL;
	pOS = NULL;
}


TSolver::~TSolver()
{
	ClearEuqations();

	if (pOS != NULL)
		delete pOS;
}

void TSolver::SetHwnd(HWND hwnd)
{
	hwndOutput = hwnd;
	if (hwnd != NULL)
	{
		pOS = new Ostringstream;
		pOS->imbue(std::locale(""));
	}
	else
	{
		if (pOS != NULL)
			delete pOS;
		pOS = NULL;
	}
}

void TSolver::Outputln(const TCHAR *szFormat, ...)
{
	if (pOS != NULL)
		if (szFormat != NULL && _tcslen(szFormat) > 0)
		{
			TCHAR *szBuffer = new TCHAR[_tcslen(szFormat) + 1024];
			va_list pArgList;
			va_start(pArgList, szFormat);
			_vsntprintf(szBuffer, _tcslen(szFormat) + 1024, szFormat, pArgList);
			va_end(pArgList);

			//追加换行
			*pOS << szBuffer;
			*pOS << TEXT("\r\n");

			delete[] szBuffer;
		}
}

void TSolver::Output(TCHAR *szFormat, ...)
{
	if (szFormat == NULL || pOS == NULL)
		return;
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	*pOS << szBuffer;
}

int TSolver::GetIdFromVariableStr(String varname)
{
	int i = varname.find_first_of(TEXT("0123456789"));
	String temp = varname.substr(i);
	return std::stoi(temp);
}

void TSolver::ClearConstraint()
{
	Equations->RemoveTempEquations();
}


void TSolver::RecordStartDragPos(int index, DPOINT dpt)
{
	RecordStartDragPos(pShape->Element[index], dpt);
}

void TSolver::RecordStartDragPos(TElement *pElement, DPOINT dpt)
{
	DPOINT dptSPP = TDraw::GetRelative(dpt, pElement->dpt, pElement->angle);

	if (!IsZero(dptSPP.x, precision))
		dRelativeAngle = atan(dptSPP.y / dptSPP.x);
	else
		dRelativeAngle = 0;
}

//添加鼠标约束
void TSolver::AddMouseConstraint(int index, DPOINT dptm)
{
	AddMouseConstraint(pShape->Element[index],dptm);
}

//添加鼠标约束
void TSolver::AddMouseConstraint(TElement *pElement, DPOINT dptm)
{
	if (pElement->CanBeDragged())
	{
		TCHAR temp[200];
		double xm = dptm.x;
		double ym = dptm.y;
		int id = pElement->id;

		_stprintf(temp, TEXT("(%f-x%d)*sin(phi%d+%f)-(%f-y%d)*cos(phi%d+%f)"),xm, id,  id, dRelativeAngle, ym,  id,id, dRelativeAngle);

		TCHAR szVar[100], szValue[100];
		_stprintf(szVar, TEXT("x%d y%d phi%d"), id, id, id);
		_stprintf(szValue, TEXT("%f %f %f"), pElement->dpt.x, pElement->dpt.y, pElement->angle);

		Equations->DefineVariable(pOS, szVar, szValue,true);
		Equations->AddEquation(pOS, temp, true);

		return;
	}
}

void TSolver::ClearEuqations()
{
	if (Equations != NULL)
		delete Equations;
	Equations = new TEquations;

	//if (EquationsV != NULL)
	//	delete EquationsV;
	//EquationsV = new TEquations;

	vecStrDriver.clear();
}

void TSolver::RefreshEquations()
{
	//全部清理
	subsVar.clear();
	subsValue.clear();
	ClearOutput();
	ClearEuqations();

	//开始
	Outputln(TEXT("自由度: DOF = nc - nh = nb*3 - nh = %d*3 - %d = %d"), pShape->nb, pShape->nh(), pShape->DOF());
	int i, j;
	DPOINT SiP, SjP, SiQ, SjQ;
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
			//_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			//_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			//Equations->DefineVariable(pOS, buffer1, buffer2);
			Equations->DefineOneVariable(pOS, TEXT("x") + To_string(i), xi,true);
			Equations->DefineOneVariable(pOS, TEXT("y") + To_string(i), yi, true);
			Equations->DefineOneVariable(pOS, TEXT("phi") + To_string(i), phii, true);
			Equations->DefineOneVariable(pOS, TEXT("x") + To_string(j), xj, true);
			Equations->DefineOneVariable(pOS, TEXT("y") + To_string(j), yj, true);
			Equations->DefineOneVariable(pOS, TEXT("phi") + To_string(j), phij, true);

			//读入方程
			/* 推导结果：
			xi + xiP*cos(phii) - yiP*sin(phii) - xj  - xjP*cos(phij) + yjP*sin(phij)
			yi + xiP*sin(phii) + yiP*cos(phii) - yj  - xjP*sin(phij) - yjP*cos(phij)  */
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), i, SiP.x, i, SiP.y, i, j, SjP.x, j, SjP.y, j);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d-%f*sin(phi%d)-%f*cos(phi%d)"), i, SiP.x, i, SiP.y, i, j, SjP.x, j, SjP.y, j);
			Equations->AddEquation(pOS, buffer1, false);
			Equations->AddEquation(pOS, buffer2, false);
			break;
		}
		case CONSTRAINT_COLINEAR://共线约束
		{
			TConstraintColinear *pColinear = (TConstraintColinear *)element;
			//构件i
			pShape->GetSP(pColinear->pElement[0], pColinear->PointBeginIndexOfElement[0], SiP, i);//xiP,yiP
			pShape->GetSQ(pColinear->pElement[0], pColinear->PointEndIndexOfElement[0], SiQ, i);//xiQ,yiQ

			//构件j
			pShape->GetSP(pColinear->pElement[1], pColinear->PointBeginIndexOfElement[1], SjP, j);//xjP,yjP
			pShape->GetSQ(pColinear->pElement[1], pColinear->PointEndIndexOfElement[1], SjQ, j);//xjQ,yjQ

			//得到2个重合构件的广义坐标
			double xi, yi, phii, xj, yj, phij;
			pShape->GetCoordinateByElement(pColinear->pElement[0], &xi, &yi, &phii);
			pShape->GetCoordinateByElement(pColinear->pElement[1], &xj, &yj, &phij);

			//定义变量及其初始值
			//_stprintf(buffer1, TEXT("x%d y%d phi%d x%d y%d phi%d"), i, i, i, j, j, j);
			//_stprintf(buffer2, TEXT("%f %f %f %f %f %f"), xi, yi, phii, xj, yj, phij);
			//Equations->DefineVariable(pOS, buffer1, buffer2, true);
			Equations->DefineOneVariable(pOS, TEXT("x") + To_string(i), xi, true);
			Equations->DefineOneVariable(pOS, TEXT("y") + To_string(i), yi, true);
			Equations->DefineOneVariable(pOS, TEXT("phi") + To_string(i), phii, true);
			Equations->DefineOneVariable(pOS, TEXT("x") + To_string(j), xj, true);
			Equations->DefineOneVariable(pOS, TEXT("y") + To_string(j), yj, true);
			Equations->DefineOneVariable(pOS, TEXT("phi") + To_string(j), phij, true);

			//读入方程
			//推导结果：
			/*
			(cos(phii)*(yiP - yiQ) + sin(phii)*(xiP - xiQ))*(xi - xj + xiP*cos(phii) - xjP*cos(phij) - yiP*sin(phii) + yjP*sin(phij))
			- (cos(phii)*(xiP - xiQ) - sin(phii)*(yiP - yiQ))*(yi - yj + yiP*cos(phii) - yjP*cos(phij) + xiP*sin(phii) - xjP*sin(phij))
			*/
			_stprintf(buffer1, TEXT("(cos(phi%d)*(%f-%f)+sin(phi%d)*(%f-%f))*(x%d-x%d+%f*cos(phi%d)-%f*cos(phi%d)-%f*sin(phi%d)+%f*sin(phi%d))\
																										-(cos(phi%d)*(%f-%f)-sin(phi%d)*(%f-%f))*(y%d-y%d+%f*cos(phi%d)-%f*cos(phi%d)+%f*sin(phi%d)-%f*sin(phi%d))"),
																										i, SiP.y, SiQ.y, i, SiP.x, SiQ.x, i, j, SiP.x, i, SjP.x, j, SiP.y, i, SjP.y, j,
																										i, SiP.x, SiQ.x, i, SiP.y, SiQ.y, i, j, SiP.y, i, SjP.y, j, SiP.x, i, SjP.x, j);

			_stprintf(buffer2, TEXT("(cos(phi%d-phi%d)*(%f-%f)+sin(phi%d-phi%d)*(%f-%f))*(%f-%f)-(cos(phi%d-phi%d)*(%f-%f)-sin(phi%d-phi%d)*(%f-%f))*(%f-%f)"),
				i, j, SiP.y, SiQ.y, i, j, SiP.x, SiQ.x, SjP.x, SjQ.x, i, j, SiP.x, SiQ.x, i, j, SiP.y, SiQ.y, SjP.y, SjQ.y);

			Equations->AddEquation(pOS, buffer1, false);
			Equations->AddEquation(pOS, buffer2, false);
			break;
		}
		case ELEMENT_SLIDEWAY:
		case ELEMENT_FRAMEPOINT:
		{
			int id = element->id;

			subsVar.push_back(TEXT("x") + To_string(id));
			subsVar.push_back(TEXT("y") + To_string(id));
			subsVar.push_back(TEXT("phi") + To_string(id));
			subsValue.push_back(element->dpt.x);
			subsValue.push_back(element->dpt.y);
			subsValue.push_back(element->angle);
			//Solve时，建立Jacobian会替换掉
			break;
		}
		case DRIVER:
		{
			TDriver *pDriver = (TDriver *)element;
			String s;
			s += pDriver->sExprLeft + TEXT("-(") + pDriver->sExprRight+TEXT(")");
			vecStrDriver.push_back(s);
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

	if (pOS != NULL)
		RefreshWindowText();
}

void TSolver::ClearOutput()
{
	if (pOS != NULL)
	{
		delete pOS;
		pOS = new Ostringstream;
		pOS->imbue(std::locale(""));
	}
}

//必须使用了本函数才刷新Edit内容
void TSolver::RefreshWindowText()
{
	if (hwndOutput != NULL)
	{
		SetWindowText(hwndOutput, ((Ostringstream*)pOS)->str().c_str());

		//SetFocus(Edit.m_hWnd);

		int len = ::GetWindowTextLength(hwndOutput);
		::PostMessage(hwndOutput, EM_SETSEL, len, len);
		PostMessage(hwndOutput, EM_SCROLLCARET, 0, 0);
	}
}

//将Shape的Element设置为VariableTable定义的数值
void TSolver::SetElementDisplacement(const TVariableTable &VariableTable)
{
	for (size_t i = 0; i < VariableTable.VariableTable.size(); i++)
	{
		int id = 0;
		try
		{
			id = GetIdFromVariableStr(VariableTable.VariableTable[i]);
		}
		catch (std::exception &err)
		{
			//var is not a number
			continue;
		}
		TElement *element = pShape->GetElementById(id);
		const double &data = VariableTable.VariableValue[i];

		switch (VariableTable.VariableTable[i][0])
		{
		case TEXT('x'):
			element->SetX(data);
			break;
		case TEXT('y'):
			element->SetY(data);
			break;
		case TEXT('p'):
			element->SetPhi(data);//MakeIn2Pi
			break;
		default:
			continue;
		}
	}
}

//区分x y phi，按照变量表的顺序，将vecpValue链接到元素的值
void TSolver::LinkpValue(std::vector<double *> &vecpValue)
{
	vecpValue.clear();
	TVariableTable &VariableTable = Equations->VariableTable;
	for (size_t i = 0; i < VariableTable.VariableTable.size(); i++)
	{
		int id = GetIdFromVariableStr(VariableTable.VariableTable[i]);
		TElement *element = pShape->GetElementById(id);

		switch (VariableTable.VariableTable[i][0])
		{
		case TEXT('x'):
			vecpValue.push_back(&(element->dpt.x));
			break;
		case TEXT('y'):
			vecpValue.push_back(&(element->dpt.y));
			break;
		case TEXT('p'):
			vecpValue.push_back(&(element->angle));
			break;
		default:
			continue;
		}

	}
}

void TSolver::GetResult(std::vector<double> &vecResult)
{
	vecResult = Equations->VariableTable.VariableValue;
}

//求解
void TSolver::Solve()
{
	clock_t start, stop;//clock_t是clock()函数返回类型
	double duration;

	start = clock();

	SubsFramePoint();
	Equations->SimplifyEquations(pOS);
	Equations->BuildJacobian(pOS);
	Equations->SolveEquations(pOS);//解方程

	if (Equations->hasSolved)//解出
	{
		SetElementDisplacement(Equations->VariableTable);
	}

	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK;

	Outputln(TEXT("\r\n耗时 %f s"), duration);

	if (pOS != NULL)
		RefreshWindowText();
}

void TSolver::SubsFramePoint()
{
	if (!subsVar.empty() && !subsValue.empty())
		Equations->Subs(pOS, subsVar, subsValue);
}

//求解之前需要调用SubsFramePoint
//返回是否求解成功
bool TSolver::Solve(double t)
{
	if (pOS)
	{
		String s(30, TEXT('='));
		*pOS << s << TEXT(" Solve(t=") << t << TEXT(") ") << s<<TEXT("\r\n");
	}

	Equations->RemoveTempEquations();

	Equations->DefineOneVariable(pOS, TEXT("t"), t,true);

	SubsFramePoint();

	//加入驱动方程
	for (auto StrDriver : vecStrDriver)
		Equations->AddEquation(pOS, StrDriver, true);

	Equations->BuildEquationsV(pOS);//此时t还在变量组内
	Equations->BuildEquationsA_Phitt(pOS);

	Equations->Subs(pOS, TEXT("t"), t);//方程组代入t
	//Equations->SimplifyEquations(pOS);

	Equations->BuildVariableTableV(pOS);
	Equations->BuildVariableTableA(pOS);

	Equations->BuildJacobian(pOS);

	//解出位移方程
	Equations->SolveEquations(pOS);

	if (Equations->hasSolved)
	{
		//解出速度方程
		Equations->SubsV(pOS, TEXT("t"), t);
		Equations->SolveEquationsV(pOS);

		//解出加速度方程
		Equations->SubsA(pOS, TEXT("t"), t);
		Equations->SolveEquationsA(pOS);

		//设置位移
		SetElementDisplacement(Equations->VariableTable);
	}
	RefreshWindowText();

	return Equations->hasSolved;
}

void TSolver::GetMesureResult(std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex)
{
	for (size_t i = 0; i < vecItems.size(); ++i)
	{
		TVariableTable *pVarTable;
		switch (vecItems[i].type)
		{
			using namespace DialogAnimation;
		case D:
		{
			pVarTable = &(Equations->VariableTableUnsolved); //指向VariableTable

			DPOINT dptAb=vecItems[i].pElement->GetAbsolutePointByIndex(vecItems[i].index_of_point);
			switch (vecItems[i].value_type)
			{
			case X:
				vecItems[i].data.push_back(dptAb.x); break;
			case Y:
				vecItems[i].data.push_back(dptAb.y); break;
			case PHI:
				vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]); break;
			}
			break;
		}
		case V:
			pVarTable = &(Equations->VariableTableV);//指向VariableTableV
			vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]);
			break;
		case A:
			pVarTable = &(Equations->VariableTableA);//指向VariableTableA
			vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]);
			break;
		}
	}
}

//以未解出变量表为顺序依据，调用前必须SubsFramePoint
void TSolver::LinkMesureResult(const std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex)
{
	String temp;
	for (auto &Item : vecItems)
	{
		switch (Item.value_type)
		{
			using namespace DialogAnimation;
		case X:temp=TEXT("x"); break;
		case Y:temp = TEXT("y"); break;
		case PHI:temp = TEXT("phi"); break;
		}
		temp+=To_string(Item.id);
		//至此得到变量名

		//位移表和速度表项目顺序一致，得到变量索引
		TVariableTable *pVarTable = &(Equations->VariableTableUnsolved);//以未解出变量表顺序为准

		auto it = find(pVarTable->VariableTable.begin(), pVarTable->VariableTable.end(), temp);
		if (it != pVarTable->VariableTable.end())
		{
			vecIndex.push_back(it - pVarTable->VariableTable.begin());
		}
	}

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

	/*
	Outputln(Equations->DefineVariable(true, TEXT("x2 phi2 z t l"), TEXT("0 0 0 0 0")));
	Outputln(Equations->AddEquation(true, TEXT("-x2+1+1.3*sin(phi2)*l"), false));
	Outputln(Equations->AddEquation(true, TEXT("-1.3*(-phi2/(5+4))-20"), false));
	Outputln(Equations->AddEquation(true, TEXT("2*sin(z)+0.5*cos(z)+t"), false));
	Outputln(Equations->AddEquation(true, TEXT("t-0.3"), false));
	Outputln(Equations->Subs(true, TEXT("l"), TEXT("1.3")));
	Outputln(Equations->SimplifyEquations(true));
	Outputln(Equations->BuildJacobian(true));
	Outputln(Equations->SolveEquations(true));*/

	Equations->RemoveTempEquations();

	Equations->DefineOneVariable(pOS, TEXT("t"), 0.1);
	Equations->AddEquation(pOS, TEXT("phi2-ln(t)"), true);
	Equations->Subs(pOS, subsVar, subsValue);

	Equations->BuildEquationsV(pOS);//此时t还在变量组内
	Equations->BuildEquationsA_Phitt(pOS);//

	Equations->Subs(pOS, TEXT("t"), 0.1);
	//Equations->SimplifyEquations(pOS);

	Equations->BuildVariableTableV(pOS);//
	Equations->BuildVariableTableA(pOS);//

	Equations->BuildJacobian(pOS);

	//解出位移方程
	Equations->SolveEquations(pOS);

	if (Equations->hasSolved)
	{
		//解出速度方程
		Equations->SubsV(pOS, TEXT("t"), 0.1);//
		Equations->SolveEquationsV(pOS);//

		//解出加速度方程
		Equations->SubsA(pOS, TEXT("t"), 0.1);//
		Equations->SolveEquationsA(pOS);

		//设置位移
		SetElementDisplacement(Equations->VariableTable);
	}
	RefreshWindowText();

	//EquationsV->RemoveTempEquations();
	//EquationsV->DefineVariable(pOS, TEXT("t"), TEXT("0"));
	//EquationsV->AddEquation(pOS, TEXT("phi2-sin(t)"), true);
	//EquationsV->Subs(pOS, subsVar, subsValue);
	//EquationsV->BuildJacobian(pOS);
	//for (auto pEqua : Equations->Equations)
	//{
	//	Outputln(pEqua->Diff(TEXT("t"), 1, true));
	//	Outputln(pEqua->Simplify(true));
	//}
	//Equations->SolveEquations(pOS);

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