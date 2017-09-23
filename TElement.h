#pragma once
#include <vector>
#include <Windows.h>

#include "String.h"

#include "DPOINT.h"

enum EnumElementType{
	ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR,
	ELEMENT_POLYLINEBAR,
	DRIVER
};

enum EnumElementClass{
	ELEMENT_CLASS_FRAME,
	ELEMENT_CLASS_NORMAL,
	ELEMENT_CLASS_CONSTRAINT,
	ELEMENT_CLASS_DRIVER,
	ELEMENT_CLASS_NONE
};

class TConfiguration;
class TShape;
class TListView;
class TElement
{
	friend void RefreshDOF(TElement *pElement, int &nb, int &iCoincideNum, int &iDriverNum, int &iFrameNum, bool isAdd);
private:
protected:
	virtual bool TElement::InSelWindow(RECT rect, const TConfiguration *pConfig){ return false; }
	virtual bool TElement::InSelCross(RECT rect, const TConfiguration *pConfig){ return false; }
public:
	int id;
	bool available;//未启用
	EnumElementType eType;//类型
	EnumElementClass eClass;
	TCHAR Name[64];//名称
	LOGPEN logpenStyleShow, logpenStyle;//显示样式，本身样式

	std::vector<DPOINT> vecDpt;//点集
	std::vector<std::vector<int>> vecIsJoint;

	DPOINT dpt;
	double angle;

	bool bDrawSquare;
	unsigned char alpha;

	TElement();
	virtual ~TElement();
	//virtual void TElement::BuildpDpt();
	virtual const String TElement::GetElementTypeName()=0;//得到类型名称
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos)=0;
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)=0;
	virtual void TElement::Draw(HDC hdc, const TConfiguration* pConfig) = 0;
	virtual void TElement::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) = 0;
	virtual void TElement::NoticeListView(TListView *pListView)=0;
	virtual const DPOINT TElement::GetRelativePointByIndex(int PointIndexOfElement) const;
	virtual DPOINT TElement::GetAbsolutePointByIndex(int PointIndexOfElement) const;
	virtual void TElement::SetX(double x);
	virtual void TElement::SetY(double y);
	virtual void TElement::SetPhi(double phi);
	virtual void TElement::ChangePos(DPOINT dptDelta);
	virtual bool TElement::Picked(const POINT &ptPos, const TConfiguration *pConfig){ return false; }
	virtual bool TElement::InSelect(RECT rect, bool bSelCross, const TConfiguration *pConfig);
	virtual void TElement::SetStateNormal();
	virtual void TElement::SetStateHover();
	virtual void TElement::SetStateUnHover();
	virtual void TElement::SetStateChosen();

	bool TElement::IsConstraint(){ return eClass == ELEMENT_CLASS_CONSTRAINT; }
	bool TElement::CanBeDragged(){ return eClass == ELEMENT_CLASS_NORMAL; }

	void TElement::SetStyle(const LOGPEN &logpen);//设置样式
	void TElement::SetStyle(TConfiguration *pConfig);//设置样式
	void TElement::SetColor(COLORREF cr);
	void TElement::SetLineWidth(LONG width);

	TElement& TElement::operator=(const TElement &element);
};

	const String GetLineStyleName(UINT linestyle);//得到线型名称
	DPOINT GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);
	void RegularRect(RECT &rect);
	bool RectInRect(const RECT &rcBig, const RECT &rcSmall);
	bool RectCrossRect(const RECT &rcBig, const RECT &rcSmall);
	bool APointInRect(const RECT &rect, POINT *apt, int count);
	bool APointCrossRect(const RECT &rect, POINT *apt, int count);
	bool VecPointInRect(const RECT &rect, std::vector<POINT> &vecpt);
	bool VecPointCrossRect(const RECT &rect, std::vector<POINT> &vecpt);
	void RefreshDOF(TElement *pElement, int &nb,int &iCoincideNum,int &iDriverNum, int &iFrameNum,bool isAdd);