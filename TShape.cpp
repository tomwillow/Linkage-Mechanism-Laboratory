#pragma once
#include "DetectMemoryLeak.h"

#include "TSlider.h"

#include "TShape.h"
#include "TConfiguration.h"

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

TElement * TShape::GetElementById(int id)
{
	for (int i = 0; i < Element.size(); i++)
	{
		if (Element[i]->id == id)
			return Element[i];
	}
}

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

	std::vector<int> DeletedId = { Element[index]->id };

	TElement *pDeleted = Element[index];//得到被删元素的地址
	if (pDeleted->eType == CONSTRAINT_COINCIDE)
		((TConstraintCoincide *)pDeleted)->RestorePointStyle();

	//依次比对 约束 中是否有被删元素地址，有则删掉约束
	for (auto iter = Element.begin(); iter != Element.end();)
	{
		if ((*iter)->eType==CONSTRAINT_COINCIDE)//如果是约束
			if (((TConstraintCoincide *)*iter)->pElement[0] == pDeleted ||
				((TConstraintCoincide *)*iter)->pElement[1] == pDeleted
				)//且涉及到被删除元素，则删除此约束
			{
				iCoincideNum--;
				DeletedId.push_back((*iter)->id);

				((TConstraintCoincide *)(*iter))->RestorePointStyle();
				delete *iter;
				iter=Element.erase(iter);
				continue;
			}
		iter++;
	}

	delete pDeleted;
	auto iter = Element.begin() + index;
	Element.erase(iter);

	return DeletedId;
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

void TShape::GetCoordinateById(int id, double *x, double *y, double *theta)
{
	TElement *temp = GetElementById(id);
	GetCoordinateByElement(temp, x, y, theta);
}

void TShape::ChangePos(int index, DPOINT dptDelta)
{
	TElement *temp = Element[index];
	switch (temp->eType)
	{
	case ELEMENT_SLIDER:
	case ELEMENT_FRAMEPOINT:
		((TFramePoint *)temp)->dpt.x += dptDelta.x;
		((TFramePoint *)temp)->dpt.y += dptDelta.y;
		break;
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_SLIDEWAY:
		((TRealLine *)temp)->ptBegin.x += dptDelta.x;
		((TRealLine *)temp)->ptBegin.y += dptDelta.y;
		((TRealLine *)temp)->ptEnd.x += dptDelta.x;
		((TRealLine *)temp)->ptEnd.y += dptDelta.y;
		break;
	default:
		assert(0);
		break;
	}
}

void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta)
{
	GetCoordinateByElement(Element[index], x, y, theta);
}

void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta)
{
	switch (element->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		*x = ((TBar *)element)->ptBegin.x;
		*y = ((TBar *)element)->ptBegin.y;
		*theta = ((TBar *)element)->dAngle;
		break;
	case ELEMENT_FRAMEPOINT:
		*x = element->dpt.x;
		*y = element->dpt.y;
		*theta = 0;
		break;
	case ELEMENT_SLIDER:
		*x = element->dpt.x;
		*y = element->dpt.y;
		*theta = element->angle;
		break;
	default:
		assert(0);
		break;
	}
}

void TShape::GetSQ(const TElement *pElement, int PointIndexOfElement, DPOINT &SQ, int &i)
{
	i = pElement->id;
	switch (pElement->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_FRAMEPOINT://第二点
		SQ = { ((TRealLine *)pElement)->dLength, 0 };
		break;
	case ELEMENT_SLIDER:
		SQ = { 1, 0 };
		break;
	default:
		assert(0);
		break;
	}
}

void TShape::GetSP(const TElement *pElement,int PointIndexOfElement, DPOINT &SP, int &i)
{
	i = pElement->id;
	switch (pElement->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
		if (PointIndexOfElement == 1)//ptBegin
			SP = { 0, 0 };
		else
			SP = { ((TBar *)GetElementById(i))->dLength, 0 };
		break;
	case ELEMENT_FRAMEPOINT:
		SP = { 0, 0 };
		break;
	case ELEMENT_SLIDER:
		SP = ((TSlider *)pElement)->vecDpt[PointIndexOfElement];
		break;
	default:
		assert(0);
		break;
	}
}

//SiP={xi'P,yi'P}
void TShape::GetSijP(const TConstraintCoincide *pCoincide, DPOINT &SiP, DPOINT &SjP, int &i, int &j)
{
	GetSP(pCoincide->pElement[0], pCoincide->PointIndexOfElement[0], SiP, i);
	GetSP(pCoincide->pElement[1], pCoincide->PointIndexOfElement[1], SjP, j);
	return;
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
	case ELEMENT_SLIDEWAY:
		return sizeof(TSlideway);
	case CONSTRAINT_COINCIDE:
		return sizeof(TConstraintCoincide);
	case ELEMENT_SLIDER:
		return sizeof(TSlider);
	default:
		assert(0);
		break;
	}
}