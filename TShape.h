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
	int iNextId;
	std::vector<TElement *> Element;
	TShape();
	~TShape();
	void TShape::ReleaseAll();
	std::vector<int> TShape::DeleteElement(int index);
	void TShape::AddRealLine(TRealLine &realline);
	void TShape::AddBar(TBar *bar);
	void TShape::AddFramePoint(TFramePoint &framepoint);

	void TShape::AddCoincide(TConstraintCoincide &coincide);
	std::vector<int> TShape::GetInfluenceId(int id);
	void TShape::DeleteById(std::vector<int> IdArray);
	TElement * GetElementById(int id);
};

