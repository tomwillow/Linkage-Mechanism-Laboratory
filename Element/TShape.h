#pragma once

#include "..\Common\DetectMemoryLeak.h"
#include <Windows.h>
#include <vector>
#include <set>

#include "TElement.h"
#include "..\Common\DPOINT.h"

class TSlider;
class TConfiguration;
class TConstraintCoincide;
class TShape
{
private:
	HWND hParent;
	int iCoincideNum=0;
	int iFrameNum = 0;
	TElement* TShape::BuildElementByType(EnumElementType eType)const;
public:
	int iDriverNum=0;
	int nb=0;//刚性构件数
	int iNextId=0;
	std::vector<TElement *> Element;

	TShape();
	~TShape();

	template <typename T>
	T* AddElement(const T *pElement)
	{
		T *temp = new T;
		*temp = *pElement;

		if (temp->id == -1)
			temp->id = iNextId;
		else
			iNextId = temp->id;

		RefreshDOF(temp, nb, iCoincideNum, iDriverNum, iFrameNum,true);

		Element.push_back(temp);
		iNextId++;

		return temp;
	}

	//TElement* AddElement(const TElement *pElement)
	//{
	//	TElement *temp = BuildElementByType(pElement->eType);
	//	temp->operator=(*pElement);

	//	if (temp->id == -1)
	//		temp->id = iNextId;
	//	else
	//		iNextId = temp->id;

	//	RefreshDOF(temp, nb, iCoincideNum, iDriverNum, iFrameNum, true);

	//	Element.push_back(temp);
	//	iNextId++;

	//	return temp;
	//}

	void TShape::ReleaseAll();
	std::vector<int> TShape::Delete(std::set<TElement *> &DeleteLists);
	TElement* TShape::GetElementById(int id);
	int TShape::CalcFrameNum();
	int TShape::nc();
	int TShape::nh();
	int TShape::DOF();
	TElement* TShape::GetPickedElement(const POINT &ptPos, const TConfiguration *pConfig);
	void TShape::GetSP(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i);
	void TShape::GetSijP(const TConstraintCoincide *pCoincide, DPOINT &SiP, DPOINT &SjP, int &i, int &j);
	void TShape::GetSQ(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i);
	void TShape::GetCoordinateById(int id, double *x, double *y, double *theta);
	void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta);
	void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta);

	void TShape::ChangePos(int index, DPOINT dptDelta);

	bool TShape::ReadFromFile(TCHAR szFileName[]);
	bool TShape::ReadFromFile_inner(HANDLE hf);
	bool TShape::SaveToFile(TCHAR szFileName[]);

	bool TShape::GetBoundingBox(RECT &rect, POINT &center, const TConfiguration *pConfig);

	void TShape::SimplifyPhiValue();
};

