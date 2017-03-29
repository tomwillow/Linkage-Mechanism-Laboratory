#pragma once
#include "DetectMemoryLeak.h"

#include "TSlider.h"

#include "TShape.h"
#include "TConfiguration.h"

#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"

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

TElement* TShape::GetElementById(int id)
{
	for (auto pElement : Element)
	{
		if (pElement->id == id)
			return pElement;
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
		switch ((*iter)->eType)
		{
		case CONSTRAINT_COINCIDE://如果是约束
			if (((TConstraintCoincide *)*iter)->pElement[0] == pDeleted ||
				((TConstraintCoincide *)*iter)->pElement[1] == pDeleted
				)//且涉及到被删除元素，则删除此约束
			{
				iCoincideNum--;
				DeletedId.push_back((*iter)->id);

				((TConstraintCoincide *)(*iter))->RestorePointStyle();
				delete *iter;
				iter = Element.erase(iter);
				continue;
			}
		case CONSTRAINT_COLINEAR:
			if (((TConstraintColinear *)*iter)->pElement[0] == pDeleted ||
				((TConstraintColinear *)*iter)->pElement[1] == pDeleted
				)//且涉及到被删除元素，则删除此约束
			{
				DeletedId.push_back((*iter)->id);

				delete *iter;
				iter = Element.erase(iter);
				continue;
			}
		}
		++iter;
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
	case ELEMENT_SLIDEWAY:
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
	case ELEMENT_SLIDEWAY:
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

void TShape::GetSP(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i)
{
	i = pElement->id;
	switch (pElement->eType)
	{
	case ELEMENT_BAR:
	case ELEMENT_REALLINE:
	case ELEMENT_FRAMEPOINT://不可能出现1
	case ELEMENT_SLIDEWAY:
		if (PointIndexOfElement == 0)//ptBegin
			SP = { 0, 0 };
		else
			SP = { ((TBar *)GetElementById(i))->dLength, 0 };
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
	case ELEMENT_SLIDER:
		return sizeof(TSlider);
	case CONSTRAINT_COINCIDE:
		return sizeof(TConstraintCoincide);
	case CONSTRAINT_COLINEAR:
		return sizeof(TConstraintColinear);
	default:
		assert(0);
		break;
	}
}

bool TShape::ReadFromFile(TCHAR szFileName[])
{
	HANDLE hf;
	hf = CreateFile(szFileName,
		GENERIC_READ,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (GetLastError() != 0)
		return false;

	//读入元素数量
	DWORD now_pos = 0;
	int size;
	ReadFile(hf, &size, sizeof(size), &now_pos, NULL);
	if (GetLastError() != 0)
		return false;
	now_pos += sizeof(size);

	EnumElementType eType;
	for (int i = 0; i < size; i++)
	{
		//读入类型
		ReadFile(hf, &eType, sizeof(EnumElementType), &now_pos, NULL);
		if (GetLastError() != 0)
			return false;
		now_pos += sizeof(EnumElementType);

		//按照类型读入元素
		switch (eType)
		{
		case ELEMENT_BAR:
		{
			TBar temp;
			if (temp.ReadFile(hf, now_pos,this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case ELEMENT_REALLINE:
		{
			TRealLine temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case ELEMENT_FRAMEPOINT:
		{
			TFramePoint temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case ELEMENT_SLIDER:
		{
			TSlider temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case ELEMENT_SLIDEWAY:
		{
			TSlideway temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case CONSTRAINT_COINCIDE:
		{
			TConstraintCoincide temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		case CONSTRAINT_COLINEAR:
		{
			TConstraintColinear temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		default:
			assert(0);
			break;
		}
	}

	CloseHandle(hf);
	return true;
}

bool TShape::SaveToFile(TCHAR szFileName[])
{

	//Ofstream out(szFileName,std::ios::binary);
	//for (auto pElement : m_Shape.Element)
	//{
	//	pElement->Save(out);
	//}
	//out.close();
	//break;
	//std::wofstream out(szFileName);
	//for (auto element : m_Shape.Element)
	//{
	//	std::vector<String> vec;
	//	vec.push_back(TEXT("aaaa"));
	//	out << vec[0];
	//	//out << *element;
	//	out.close();
	//}

	//无弹窗直接存储
	HANDLE hf;
	hf = CreateFile(szFileName,
		GENERIC_WRITE,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	DWORD now_pos = 0;

	//写入元素数量
	int size = Element.size();
	WriteFile(hf, &size, sizeof(size), &now_pos, NULL);
	now_pos += sizeof(size);
	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;

	//逐条写入Element
	for (auto pElement : Element)
	{
		if (!pElement->WriteFile(hf, now_pos))
			return false;
	}
	CloseHandle(hf);

	return true;
}