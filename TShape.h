#pragma once
#include <Windows.h>
#include <vector>

#include "TBar.h"
#include "TRealLine.h"
#include "TFramePoint.h"
#include "TSlideway.h"
#include "DPOINT.h"

class TSlider;
class TConfiguration;
class TConstraintCoincide;
class TShape
{
private:
public:
	int iCoincideNum;
	int nb;//刚性构件数
	int hasFrame;
	int iNextId;
	std::vector<TElement *> Element;
	TShape();
	~TShape();
	void TShape::ReleaseAll();
	std::vector<int> TShape::DeleteElement(int index);
	TElement * TShape::AddRealLine(TRealLine &realline);
	TElement * TShape::AddBar(TBar *bar);
	TElement * TShape::AddFramePoint(TFramePoint &framepoint);
	void TShape::AddElement(TElement *element);
	TElement * TShape::AddSlideway(TSlideway *slideway);
	TElement * TShape::AddSlider(TSlider *slider);

	void TShape::AddCoincide(TConstraintCoincide &coincide, TConfiguration *pConfig);
	TElement * GetElementById(int id);
	int TShape::CalcFrameNum();
	int TShape::nc();
	int TShape::nh();
	int TShape::DOF();
	void TShape::GetSijP(TElement *element, DPOINT *SiP, DPOINT *SjP, int *i, int *j);
	void TShape::GetCoordinateById(int id, double *x, double *y, double *theta);
	void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta);
	void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta);
	DWORD TShape::GetSizeOfElement(EnumElementType eType);
	void TShape::ChangePos(int index, DPOINT dptDelta);
};

