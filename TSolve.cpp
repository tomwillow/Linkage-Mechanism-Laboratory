#include "TSolve.h"

#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"

TSolve::TSolve()
{
	//str = NULL;
	str = (TCHAR *)malloc( 3 * sizeof(TCHAR));
	_tcscpy(str, TEXT(">>"));
}


TSolve::~TSolve()
{
	if (str != NULL)
		free(str);
}

void TSolve::SetHwnd(HWND hwnd)
{
	hwndOutput = hwnd;
}

void TSolve::Output(TCHAR szFormat[], ...)
{
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
	str = (TCHAR *)realloc(str, (str_num + buffer_num+3)*sizeof(TCHAR));
	_tcscat(str, szBuffer);
	_tcscat(str, TEXT("\r\n"));

	SetWindowText(hwndOutput, str);
}

void TSolve::Solute()
{
	Output(TEXT("DOF=nc-nh: %d=%d-%d"), pShape->DOF(), pShape->nc(), pShape->nh());
	int i, j;
	DPOINT SiP, SjP;
	TCHAR buffer[1000] = TEXT("\0"), buffer1[100], buffer2[100];
	TExpressionTree temp;
	for (int index = 0; index < pShape->Element.size(); index++)
	{
		if (pShape->Element[index]->eType == CONSTRAINT_COINCIDE)
		{
			pShape->GetSijP(index, &SiP, &SjP, &i, &j);
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d+%f*sin(phi%d)-%f*cos(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer, TEXT("%s\r\n%s\r\n%s"), buffer, buffer1, buffer2);
			Output(temp.Read(buffer1, false));
			Output(temp.Simplify(true));
			Output(temp.Read(buffer2, false));
			Output(temp.Simplify(true));
			//_tcscpy(buffer, TEXT(""));
			//_tcscat(buffer, buffer1);
			//_tcscat(buffer, TEXT("\n"));
			//_tcscat(buffer, buffer2);
		}
		if (pShape->Element[index]->eType == ELEMENT_FRAMEPOINT)
		{
			_stprintf(buffer1, TEXT("x%d-%f"), pShape->Element[index]->id, ((TFramePoint *)(pShape->Element[index]->id, pShape->Element[index]))->dpt.x);
			_stprintf(buffer2, TEXT("y%d-%f"), pShape->Element[index]->id, ((TFramePoint *)(pShape->Element[index]->id, pShape->Element[index]))->dpt.y);
			_stprintf(buffer, TEXT("%s\r\n%s\r\n%s"), buffer, buffer1, buffer2);
			Output(temp.Read(buffer1, false));
			Output(temp.Simplify(true));
			Output(temp.Read(buffer2, false));
			Output(temp.Simplify(true));
		}
	}
	//Output(buffer);
	TExpressionTree ex;
	Output(ex.Read(TEXT("x^sin(x)"), true));
	Output(ex.Simplify(true));
	Output(ex.Diff(TEXT("x"), 1, true));
	Output(ex.Simplify(true));
	Output(ex.Subs(TEXT("x,y"), TEXT(" 1.24*y,2.1 ")));
	Output(ex.Simplify(true));
}