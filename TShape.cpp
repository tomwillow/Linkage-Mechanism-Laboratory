#pragma once
#include "TShape.h"

#include "TConstraintCoincide.h"

TShape::TShape()
{
	iNextId = 0;
	nb = 0;
	hasFrame = false;
	iCoincideNum = 0;
}

TShape::~TShape()
{
	ReleaseAll();
}

void TShape::AddCoincide(TConstraintCoincide &coincide)
{
	TConstraintCoincide *tempcoincide = new TConstraintCoincide;
	*tempcoincide = coincide;
	if (tempcoincide->id == -1)
		tempcoincide->id = iNextId;
	else
		iNextId = tempcoincide->id;
	Element.push_back(tempcoincide);
	iNextId++;
	iCoincideNum++;
}

void TShape::AddRealLine(TRealLine &realline)
{
	TRealLine *tempRealLine = new TRealLine;
	*tempRealLine = realline;
	if (tempRealLine->id == -1)
		tempRealLine->id = iNextId;
	else
		iNextId = tempRealLine->id;
	Element.push_back(tempRealLine);
	iNextId++;
}

void TShape::AddBar(TBar *bar)
{
	TBar *tempBar = new TBar;
	*tempBar = *bar;
	if (tempBar->id == -1)
		tempBar->id = iNextId;
	else
		iNextId = tempBar->id;
	Element.push_back(tempBar);
	iNextId++;

	nb++;
}

void TShape::AddFramePoint(TFramePoint &framepoint)
{
	TFramePoint *tempFramePoint = new TFramePoint;
	*tempFramePoint = framepoint;
	if (tempFramePoint->id == -1)
		tempFramePoint->id = iNextId;
	else
		iNextId = tempFramePoint->id;
	Element.push_back(tempFramePoint);
	iNextId++;
	if (hasFrame == 0)//只加一次
	{
		hasFrame = 1;
		nb++;
	}
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

//void TShape::DeleteById(std::vector<int> IdArray)
//{
//	
//}

int TShape::CalcFrameNum()
{
	int num = 0;
	for (int i = 0; i < Element.size(); i++)
	{
		if (Element[i]->eType == ELEMENT_FRAMEPOINT)
			num++;
	}
	return num;
}

std::vector<int> TShape::DeleteElement(int index)
{
	//刷新构件及机架数量
	switch (Element[index]->eType)
	{
	case ELEMENT_BAR:
		nb--;
		break;
	case ELEMENT_FRAMEPOINT:
		if (CalcFrameNum() == 1)//最后一个机架
		{
			hasFrame = false;
			nb--;
		}
		break;
	}

	//先记下id，否则删掉就失效了
	int id = Element[index]->id;

	//删掉元素
	delete Element[index];
	std::vector<TElement *>::iterator iter = Element.begin() + index;
	Element.erase(iter);

	//清除掉和该元素相关的约束
	std::vector<int> InfluenceId;

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

				iCoincideNum--;
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
	nb = 0;
	hasFrame = false;
	iCoincideNum = 0;
}

int TShape::nc()
{
	return nb * 3;
}

int TShape::nh()
{
	return iCoincideNum * 2 + (hasFrame ? 3 : 0);
}

int TShape::DOF()
{
	return nc() - nh();
}

void TShape::GetCoordinate(int id, double *x, double *y, double *theta)
{
	TElement *temp = GetElementById(id);
	switch (temp->eType)
	{
	case ELEMENT_BAR:
		*x = ((TBar *)temp)->ptBegin.x;
		*y = ((TBar *)temp)->ptBegin.y;
		*theta = ((TBar *)temp)->dAngle;
		break;
	case ELEMENT_FRAMEPOINT:
		*x = ((TFramePoint *)temp)->dpt.x;
		*y = ((TFramePoint *)temp)->dpt.y;
		*theta = 0;
		break;
	}
}

//SiP={xi'P,yi'P}
void TShape::GetSijP(int index,DPOINT *SiP,DPOINT *SjP,int *i,int *j)
{
	TConstraintCoincide *pCoincide = (TConstraintCoincide *)Element[index];
	*i = pCoincide->ElementId1;
	*j = pCoincide->ElementId2;
	//节点i
	switch (pCoincide->eElementType1)
	{
	case ELEMENT_BAR:
		if (pCoincide->Element1PointIndex == 1)//ptBegin
			*SiP = { 0, 0 };
		else
			*SiP = { ((TBar *)GetElementById(*i))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SiP = { ((TFramePoint *)GetElementById(*i))->dpt.x, ((TFramePoint *)GetElementById(*i))->dpt.y };
		break;
	}

	//节点j
	switch (pCoincide->eElementType2)
	{
	case ELEMENT_BAR:
		if (pCoincide->Element2PointIndex == 1)//ptBegin
			*SjP = { 0, 0 };
		else
			*SjP = { ((TBar *)GetElementById(*j))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		*SjP = { ((TFramePoint *)GetElementById(*j))->dpt.x, ((TFramePoint *)GetElementById(*j))->dpt.y };
		break;
	}
}

DWORD TShape::GetSizeOfElement(EnumElementType eType)
{
	switch (eType)
	{
	case ELEMENT_BAR:
		return sizeof(TBar);
	case ELEMENT_REALLINE:
		return sizeof(TRealLine);
	case ELEMENT_FRAMEPOINT:
		return sizeof(TFramePoint);
	case CONSTRAINT_COINCIDE:
		return sizeof(TConstraintCoincide);
	}
}