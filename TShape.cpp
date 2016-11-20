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

std::vector<int> TShape::GetInfluenceId(int id)
{
	std::vector<int> InfluenceId;
	for (int i = 0; i < Element.size(); i++)
	{
		switch (Element[i]->eType)
		{
		case CONSTRAINT_COINCIDE:
			if (((TConstraintCoincide *)Element[i])->ElementId1 == id ||
				((TConstraintCoincide *)Element[i])->ElementId2 == id)
			{
				InfluenceId.push_back(Element[i]->id);
			}
		}
	}
	return InfluenceId;
}

void TShape::DeleteById(std::vector<int> IdArray)
{
	
}

std::vector<int> TShape::DeleteElement(int index)
{
	int id = Element[index]->id;

	delete Element[index];
	std::vector<TElement *>::iterator iter = Element.begin() + index;
	Element.erase(iter);

	////将所有牵涉到的约束设为无效
	//for (int i = 0; i < Element.size(); i++)
	//{
	//	switch (Element[i]->eType)
	//	{
	//	case CONSTRAINT_COINCIDE:
	//		if (((TConstraintCoincide *)Element[i])->ElementId1 == id ||
	//			((TConstraintCoincide *)Element[i])->ElementId2 == id)
	//		{
	//			((TConstraintCoincide *)Element[i])->available = false;
	//		}
	//	}
	//}
	std::vector<int> InfluenceId;

	//再删掉元素。再清除掉和该元素相关的约束
	int i = Element.size()-1;
	while (i >=0)
	{
		switch (Element[i]->eType)
		{
		case CONSTRAINT_COINCIDE:
			if (((TConstraintCoincide *)Element[i])->ElementId1 == id ||
				((TConstraintCoincide *)Element[i])->ElementId2 == id)
			{
				InfluenceId.push_back(Element[i]->id);

				delete Element[i];
				std::vector<TElement *>::iterator iter = Element.begin() + i;
				Element.erase(iter);

			}
			break;
		}
		i--;
	}
	return InfluenceId;
}

void TShape::ReleaseAll()
{
	for (int i = 0; i < Element.size(); i++)
		delete Element[i];
	Element.swap(std::vector<TElement *>());
	iNextId = 0;
}