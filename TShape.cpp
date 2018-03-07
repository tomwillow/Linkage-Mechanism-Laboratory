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
		if (pElement->IsConstraint())
		{
			p1 = ((TConstraint *)pElement)->pElement[0];
			p2 = ((TConstraint *)pElement)->pElement[1];
			if (DeleteLists.find(p1) != DeleteLists.end() || DeleteLists.find(p2) != DeleteLists.end())
			{//当前元素链接到的约束
				DeleteLists.insert(pElement);//加入删除列表
				InfluencedId.push_back(pElement->id);
			}
		}
	}

	auto iter = Element.end();
	for (auto pElement : DeleteLists)
	{
		if ((iter = std::find(Element.begin(), Element.end(), pElement)) != Element.end())
		{
			RefreshDOF(pElement, nb, iCoincideNum, iDriverNum, iFrameNum, false);
			Element.erase(iter);
		}
	}

	return InfluencedId;
}

void TShape::ReleaseAll()
{
	for (auto pElement : Element)
		delete pElement;
	Element.clear();

	iNextId = 0;
	nb = 0;
	iFrameNum = 0;
	iCoincideNum = 0;
	iDriverNum = 0;
}

int TShape::nc()
{
	return nb * 3;
}

int TShape::nh()
{
	return iCoincideNum * 2 ;//+ (iFrameNum>0 ? 3 : 0)
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

TElement* TShape::BuildElementByType(EnumElementType eType)const
{
	//按照类型读入元素
	switch (eType)
	{
	case ELEMENT_BAR:
		return new TBar;
	case ELEMENT_REALLINE:
		 return new TRealLine;
	case ELEMENT_FRAMEPOINT:
		 return new TFramePoint;
	case ELEMENT_SLIDER:
		 return new TSlider;
	case ELEMENT_POLYLINEBAR:
		 return new TPolylineBar;
	case ELEMENT_SLIDEWAY:
		 return new TSlideway;
	case CONSTRAINT_COINCIDE:
		 return new TConstraintCoincide;
	case CONSTRAINT_COLINEAR:
		 return new TConstraintColinear;
	case DRIVER:
		 return new TDriver;
	default:
		assert(0);
		return 0;
	}
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
		TElement *pElement = BuildElementByType(eType);
		if (pElement == NULL)//类型号不正确
			return false;
		if (pElement->ReadFile(hf, now_pos, this))
		{
			//AddElement(pElement);

			if (pElement->id == -1)
				pElement->id = iNextId;
			else
				iNextId = pElement->id;

			if (RefreshDOF(pElement, nb, iCoincideNum, iDriverNum, iFrameNum, true) == false)
			{
				delete pElement;
				return false;
			}

			Element.push_back(pElement);
			iNextId++;
		}
		else
		{
			delete pElement;
			return false;
		}
	}
	return true;
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