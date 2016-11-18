#pragma once
#include "TShape.h"

#include "TConstraintCoincide.h"

TShape::TShape()
{
	iNextId = 0;
}

TShape::~TShape()
{
	ReleaseAll();
}

void TShape::AddCoincide(TConstraintCoincide &coincide)
{
	TConstraintCoincide *tempcoincide = new TConstraintCoincide;
	*tempcoincide = coincide;
	tempcoincide->id = iNextId;
	Element.push_back(tempcoincide);
	iNextId++;
}

void TShape::AddRealLine(TRealLine &realline)
{
	TRealLine *tempRealLine = new TRealLine;
	*tempRealLine = realline;
	tempRealLine->id = iNextId;
	Element.push_back(tempRealLine);
	iNextId++;
}

void TShape::AddBar(TBar *bar)
{
	TBar *tempBar = new TBar;
	*tempBar = *bar;
	tempBar->id = iNextId;
	Element.push_back(tempBar);
	iNextId++;
}

void TShape::AddFramePoint(TFramePoint &framepoint)
{
	TFramePoint *tempFramePoint = new TFramePoint;
	*tempFramePoint = framepoint;
	tempFramePoint->id = iNextId;
	Element.push_back(tempFramePoint);
	iNextId++;
}

TElement * TShape::GetElementById(int id)
{
	for (int i = 0; i < Element.size(); i++)
	{
		if (Element[i]->id == id)
			return Element[i];
	}
}

void TShape::DeleteElement(int index)
{
	//先记下id，再删掉元素。再清除掉和该元素相关的约束
	int id = Element[index]->id;

	delete Element[index];
	std::vector<TElement *>::iterator iter = Element.begin() + index;
	Element.erase(iter);

	int i = 0;
	int s = Element.size();
	while (i <= s - 1)
	{
		switch (Element[i]->eType)
		{
		case CONSTRAINT_COINCIDE:
			if (((TConstraintCoincide *)Element[i])->ElementId1 == id ||
				((TConstraintCoincide *)Element[i])->ElementId2 == id)
			{
				delete Element[i];
				std::vector<TElement *>::iterator iter = Element.begin() + i;
				Element.erase(iter);
			}
			break;
		}
		s = Element.size();
		i++;
	}
}

void TShape::ReleaseAll()
{
	for (int i = 0; i < Element.size(); i++)
		delete Element[i];
	Element.swap(std::vector<TElement *>());
	iNextId = 0;
}