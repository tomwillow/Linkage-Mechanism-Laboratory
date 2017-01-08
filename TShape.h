#pragma once
#include <Windows.h>
#include <vector>

#include "TBar.h"
#include "TRealLine.h"
#include "TFramePoint.h"
#include "DPOINT.h"

class TConstraintCoincide;
class TShape
{
private:
public:
	int iCoincideNum;
	int nb;//���Թ�����
	int hasFrame;
	int iNextId;
	std::vector<TElement *> Element;
	TShape();
	~TShape();
	void TShape::ReleaseAll();
	std::vector<int> TShape::DeleteElement(int index);
	void TShape::AddRealLine(TRealLine &realline);
	void TShape::AddBar(TBar *bar);
	void TShape::AddFramePoint(TFramePoint &framepoint);
	void TShape::AddElement(TElement *element);

	void TShape::AddCoincide(TConstraintCoincide &coincide);
	std::vector<int> TShape::GetInfluenceId(int id);
	//void TShape::DeleteById(std::vector<int> IdArray);
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

