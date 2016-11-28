#include "TSolve.h"

#include "DPOINT.h"
#include "TMainWindow.h"
#include "TShape.h"
#include "TExpressionTree.h"

TSolve::TSolve()
{
}


TSolve::~TSolve()
{
}


void TSolve::Solute()
{
	ShowMessage(TEXT("DOF=nc-nh: %d=%d-%d"), pShape->DOF(), pShape->nc(), pShape->nh());
	int i, j;
	DPOINT SiP, SjP;
	TCHAR buffer[1000] = TEXT("\0"), buffer1[100], buffer2[100];
	for (int index = 0; index < pShape->Element.size(); index++)
	{
		if (pShape->Element[index]->eType == CONSTRAINT_COINCIDE)
		{
			pShape->GetSijP(index, &SiP, &SjP, &i, &j);
			_stprintf(buffer1, TEXT("x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer2, TEXT("y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d+%f*sin(phi%d)-%f*cos(phi%d)"), j, SjP.x, j, SjP.y, j, i, SiP.x, i, SiP.y, i);
			_stprintf(buffer, TEXT("%s\n%s\n%s"), buffer, buffer1, buffer2);
			//_tcscpy(buffer, TEXT(""));
			//_tcscat(buffer, buffer1);
			//_tcscat(buffer, TEXT("\n"));
			//_tcscat(buffer, buffer2);
		}
		if (pShape->Element[index]->eType == ELEMENT_FRAMEPOINT)
		{
			_stprintf(buffer1, TEXT("x%d-%f"), pShape->Element[index]->id, ((TFramePoint *)(pShape->Element[index]->id, pShape->Element[index]))->dpt.x);
			_stprintf(buffer2, TEXT("y%d-%f"), pShape->Element[index]->id, ((TFramePoint *)(pShape->Element[index]->id, pShape->Element[index]))->dpt.y);
			_stprintf(buffer, TEXT("%s\n%s\n%s"), buffer, buffer1, buffer2);
		}
	}
	TExpressionTree ex;
	_tcscpy(buffer2, TEXT("x1+-5*-sin(7^2^3+2*5)-cos (x2-7)"));
	ShowMessage(ex.Read(buffer2));
	ShowMessage(ex.Simplify());
	//ShowMessage(ex.OutputStr());
	//ShowMessage(buffer);
}