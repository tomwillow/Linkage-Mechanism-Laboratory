#pragma once
#include "DetectMemoryLeak.h"
#include <Windows.h>
#include <vector>

#include "TBar.h"
#include "TRealLine.h"
#include "TFramePoint.h"
#include "TSlideway.h"
#include "DPOINT.h"

class TSlider;
class TConfiguration;
class TConstraintCoincide;
class TShape
{
private:
public:
	int iCoincideNum;
	int nb;//刚性构件数
	int hasFrame;
	int iNextId;
	std::vector<TElement *> Element;
	TShape();
	~TShape();
	void TShape::ReleaseAll();
	std::vector<int> TShape::DeleteElement(int index);
	template <typename T>
	T* AddElement(const T *pElement)
	{
		T *temp = new T;
		*temp = *pElement;

		if (temp->id == -1)
			temp->id = iNextId;
		else
			iNextId = temp->id;

		switch (temp->eType)
		{
		case CONSTRAINT_COLINEAR:
			break;
		case CONSTRAINT_COINCIDE:
			temp->BuildpDpt();
			iCoincideNum++;
			break;
		case ELEMENT_FRAMEPOINT:
		case ELEMENT_SLIDEWAY:
			if (hasFrame == 0)//只加一次
			{
				hasFrame = 1;
				nb++;
			}
			break;
		case ELEMENT_BAR:
		case ELEMENT_SLIDER:
			nb++;
			break;
		case ELEMENT_REALLINE:
			break;
		default:
			assert(0);
			break;
		}

		Element.push_back(temp);
		iNextId++;

		return temp;
	}
	
	TElement * GetElementById(int id);
	int TShape::CalcFrameNum();
	int TShape::nc();
	int TShape::nh();
	int TShape::DOF();
	void TShape::GetSP(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i);
	void TShape::GetSijP(const TConstraintCoincide *pCoincide, DPOINT &SiP, DPOINT &SjP, int &i, int &j);
	void TShape::GetSQ(const TElement *pElement, int PointIndexOfElement, DPOINT &SP, int &i);
	void TShape::GetCoordinateById(int id, double *x, double *y, double *theta);
	void TShape::GetCoordinateByIndex(int index, double *x, double *y, double *theta);
	void TShape::GetCoordinateByElement(TElement *element, double *x, double *y, double *theta);
	DWORD TShape::GetSizeOfElement(EnumElementType eType);
	void TShape::ChangePos(int index, DPOINT dptDelta);
};

