#pragma once
#include "DetectMemoryLeak.h"

#include <algorithm>
#include "TSlider.h"

#include "TShape.h"
#include "TDraw.h"

#include "TConfiguration.h"

#include "TFramePoint.h"
#include "TRealLine.h"
#include "TBar.h"
#include "TSlideway.h"
#include "TPolylineBar.h"
#include "TConstraintCoincide.h"
#include "TConstraintColinear.h"
#include "TDriver.h"

TShape::TShape()
{
}

TShape::~TShape()
{
	ReleaseAll();
}

//没找到则返回NULL
TElement* TShape::GetElementById(int id)
{
	for (auto pElement : Element)
	{
		if (pElement->id == id)
			return pElement;
	}
	return NULL;
}

int TShape::CalcFrameNum()
{
	int num = 0;
	for (auto pElement : Element)
	{
		if (pElement->eType == ELEMENT_FRAMEPOINT)
			num++;
	}
	return num;
}

std::vector<int> TShape::Delete(std::set<TElement *> &DeleteLists)
{
	std::vector<int> InfluencedId;
	TElement *p1, *p2;
	//先加入关联约束
	for (auto pElement : Element)
	{
		if (pElement->eType == CONSTRAINT_COINCIDE)
		{
			p1 = ((TConstraintCoincide *)pElement)->pElement[0];
			p2 = ((TConstraintCoincide *)pElement)->pElement[1];
			if (DeleteLists.find(p1) != DeleteLists.end() || DeleteLists.find(p2) != DeleteLists.end())
			{
				DeleteLists.insert(pElement);
				InfluencedId.push_back(pElement->id);
			}
		}
		//if (pElement->eType == CONSTRAINT_COLINEAR)
		//{
		//	p1 = ((TConstraintCoincide *)pElement)->pElement[0];
		//	p2 = ((TConstraintCoincide *)pElement)->pElement[1];
		//	if (DeleteLists.find(p1) != DeleteLists.end() || DeleteLists.find(p2) != DeleteLists.end())
		//	{
		//		DeleteLists.insert(pElement);
		//		InfluencedId.push_back(pElement->id);
		//	}
		//}
	}

	auto iter = Element.end();
	for (auto pElement : DeleteLists)
	{
		if ((iter = std::find(Element.begin(), Element.end(), pElement)) != Element.end())
			Element.erase(iter);
	}

	return InfluencedId;
}

//相关约束一并删掉，返回其id
std::vector<int> TShape::DeleteElement(int index)
{
	//刷新构件及机架数量
	switch (Element[index]->eType)
	{
	case ELEMENT_POLYLINEBAR:
	case ELEMENT_SLIDER:
	case ELEMENT_BAR:
		nb--;
		break;
	case ELEMENT_SLIDEWAY:
	case ELEMENT_FRAMEPOINT:
		if (CalcFrameNum() == 1)//最后一个机架
		{
			hasFrame = false;
			nb--;
		}
		break;
	case DRIVER:
		iDriverNum--;
		break;
	case ELEMENT_REALLINE:
	case CONSTRAINT_COINCIDE://之后处理
	case CONSTRAINT_COLINEAR://之后处理
		break;
	default:
		assert(0);
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
				iCoincideNum--;
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
	for (auto pElement : Element)
		delete pElement;
	Element.clear();

	iNextId = 0;
	nb = 0;
	hasFrame = false;
	iCoincideNum = 0;
	iDriverNum = 0;
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
	Element[index]->ChangePos(dptDelta);
}

void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta)
{
	GetCoordinateByElement(Element[index], x, y, theta);
}

void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta)
{
	*x = element->dpt.x;
	*y = element->dpt.y;
	*theta = element->angle;
}

void TShape::GetSQ(const TElement *pElement, int PointIndexOfElement, DPOINT &SQ, int &i)
{
	i = pElement->id;
	SQ = pElement->GetRelativePointByIndex(PointIndexOfElement);
}

//传入：pElement,点序号
//传出：SP,i
void TShape::GetSP(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i)
{
	i = pElement->id;
	SP = pElement->GetRelativePointByIndex(PointIndexOfElement);
}

//SiP={xi'P,yi'P}
void TShape::GetSijP(const TConstraintCoincide *pCoincide, DPOINT &SiP, DPOINT &SjP, int &i, int &j)
{
	GetSP(pCoincide->pElement[0], pCoincide->PointIndexOfElement[0], SiP, i);
	GetSP(pCoincide->pElement[1], pCoincide->PointIndexOfElement[1], SjP, j);
	return;
}

bool TShape::ReadFromFile(TCHAR szFileName[])
{
	HANDLE hf = CreateFile(szFileName,
		GENERIC_READ,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	bool success = ReadFromFile_inner(hf);

	CloseHandle(hf);

	if (!success)
		ReleaseAll();

	return success;
}

bool TShape::ReadFromFile_inner(HANDLE hf)
{
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
			if (temp.ReadFile(hf, now_pos, this))
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
		case ELEMENT_POLYLINEBAR:
		{
			TPolylineBar temp;
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
		case DRIVER:
		{
			TDriver temp;
			if (temp.ReadFile(hf, now_pos, this))
				AddElement(&temp);
			else
				return false;
			break;
		}
		default:
			return false;
			//assert(0);
			break;
		}
	}
}

bool TShape::SaveToFile(TCHAR szFileName[])
{

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

TElement* TShape::GetPickedElement(const POINT &ptPos, const TConfiguration *pConfig)
{
	for (auto pElement : Element)
	{
		if (pElement->Picked(ptPos, pConfig))
			return pElement;
	}
	return nullptr;
}

//得到所有元素屏幕大小的包围盒及中心坐标
bool TShape::GetBoundingBox(RECT &rect, POINT &center, const TConfiguration *pConfig)
{
	if (Element.size() == 0)
		return false;
	double x_min = DBL_MAX, x_max = -DBL_MAX, y_min = DBL_MAX, y_max = -DBL_MAX;
	DPOINT dptTemp;
	for (auto pElement : Element)
	{
		for (size_t index = 0; index < pElement->vecDpt.size(); ++index)
		{
			dptTemp = pElement->GetAbsolutePointByIndex(index);
			x_min = min(x_min, dptTemp.x);
			x_max = max(x_max, dptTemp.x);
			y_min = min(y_min, dptTemp.y);
			y_max = max(y_max, dptTemp.y);
		}
	}
	TDraw::MakeRect(rect, x_min, x_max, y_min, y_max, pConfig);
	TDraw::GetCenter(center, rect);
	return true;
}