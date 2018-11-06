#pragma once
#include "..\Common\DetectMemoryLeak.h"
#include "..\Common\MyMath.h"
#include <stdio.h>
#include <memory>

#include "..\Draw\TDraw.h"
#include "..\TConfiguration.h"
#include "..\Common\ShowMessage.h"
#include "..\Element\TElement.h"

#include "..\Control\TListView.h"
TElement::TElement() :available(true), bDrawSquare(false), eClass(ELEMENT_CLASS_NONE)
{
	id = -1;
	eType = ELEMENT_NULL;
	dpt = { 0, 0 };
	angle = 0.0;

	alpha = 100;

	_tcscpy(Name,TEXT("undefined"));

}

TElement::~TElement()
{
	
}

//void TElement::BuildpDpt()
//{ 
//	assert(0); 
//}

void TElement::SetStyle(const LOGPEN &logpen)//设置样式
{
	logpenStyleShow = logpenStyle = logpen;
}

void TElement::SetStyle(TConfiguration *pConfig)
{
	if (pConfig->bRandomColor)
		SetStyle(pConfig->GetRandomColorLogpen());
	else
		SetStyle(pConfig->logpen);
}

const String TElement::GetElementTypeName()
{
	assert(0);
	return TEXT("未定义");
}

const String GetLineStyleName(UINT linestyle)
{
	switch (linestyle)
	{
	case PS_SOLID:
		return TEXT("实线");
	case PS_DASH:
		return TEXT("线段");
	case PS_DOT:
		return TEXT("虚线");
	case PS_DASHDOT:
		return TEXT("点划线");
	case PS_DASHDOTDOT:
		return TEXT("双点划线");
	default:
		assert(0);
		return TEXT("Error");
		break;
	}
}

TElement& TElement::operator=(const TElement &element)
{
	this->id = element.id;
	this->available = element.available;
	this->eType = element.eType;
	_tcscpy(this->Name, element.Name);

	this->logpenStyle = element.logpenStyle;
	this->logpenStyleShow = element.logpenStyleShow;

	this->vecDpt = element.vecDpt;
	this->vecIsJoint = element.vecIsJoint;

	this->dpt = element.dpt;
	this->angle = element.angle;

	this->alpha = element.alpha;
	return *this;
}

bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	//写入类型标记
	::WriteFile(hf, &eType, sizeof(EnumElementType), &now_pos, NULL);
	now_pos += sizeof(EnumElementType);

	//写入数据	
	::WriteFile(hf, &id, sizeof(id), &now_pos, NULL);
	now_pos += sizeof(id);
	::WriteFile(hf, &available, sizeof(available), &now_pos, NULL);
	now_pos += sizeof(available);
	::WriteFile(hf, &Name, sizeof(Name), &now_pos, NULL);
	now_pos += sizeof(Name);

	::WriteFile(hf, &logpenStyle, sizeof(logpenStyle), &now_pos, NULL);
	now_pos += sizeof(logpenStyle);
	::WriteFile(hf, &logpenStyleShow, sizeof(logpenStyleShow), &now_pos, NULL);
	now_pos += sizeof(logpenStyleShow);

	size_t tempSize = vecDpt.size();
	::WriteFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
	now_pos += sizeof(tempSize);
	for (auto Dpt : vecDpt)
	{
		::WriteFile(hf, &Dpt, sizeof(Dpt), &now_pos, NULL);
		now_pos += sizeof(Dpt);
	}

	tempSize = vecIsJoint.size();
	::WriteFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
	now_pos += sizeof(tempSize);
	for (auto JointList : vecIsJoint)
	{
		tempSize = JointList.size();
		::WriteFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
		now_pos += sizeof(tempSize);
		for (auto JointId : JointList)
		{
			::WriteFile(hf, &JointId, sizeof(JointId), &now_pos, NULL);
			now_pos += sizeof(JointId);
		}
	}

	::WriteFile(hf, &dpt, sizeof(dpt), &now_pos, NULL);
	now_pos += sizeof(dpt);
	::WriteFile(hf, &angle, sizeof(angle), &now_pos, NULL);
	now_pos += sizeof(angle);

	::WriteFile(hf, &alpha, sizeof(alpha), &now_pos, NULL);
	now_pos += sizeof(alpha);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape)
{
	//读入数据，除了eType	
	::ReadFile(hf, &id, sizeof(id), &now_pos, NULL);
	now_pos += sizeof(id);
	::ReadFile(hf, &available, sizeof(available), &now_pos, NULL);
	now_pos += sizeof(available);
	::ReadFile(hf, &Name, sizeof(Name), &now_pos, NULL);
	now_pos += sizeof(Name);

	::ReadFile(hf, &logpenStyle, sizeof(logpenStyle), &now_pos, NULL);
	now_pos += sizeof(logpenStyle);
	::ReadFile(hf, &logpenStyleShow, sizeof(logpenStyleShow), &now_pos, NULL);
	now_pos += sizeof(logpenStyleShow);

	size_t tempSize;
	DPOINT tempDpt;
	::ReadFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
	//int err_code = GetLastError();
	now_pos += sizeof(tempSize);
	vecDpt.clear();
	for (size_t i = 0; i < tempSize;++i)
	{
		::ReadFile(hf, &tempDpt, sizeof(tempDpt), &now_pos, NULL);
		now_pos += sizeof(tempDpt);
		vecDpt.push_back(tempDpt);
	}

	::ReadFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
	if (tempSize > 0x10000)
		return false;
	//err_code = GetLastError();
	//if ((err_code) != 0)
	//{
	//	ShowErrorMsgBox(TEXT("读取文件"), err_code);
	//	return false;
	//}
	now_pos += sizeof(tempSize);
	size_t JointNum;
	int JointId;
	vecIsJoint.clear();
	std::vector<int> JointList;
	for (size_t i = 0; i < tempSize;++i)
	{
		::ReadFile(hf, &JointNum, sizeof(JointNum), &now_pos, NULL);
		now_pos += sizeof(JointNum);
		for (size_t n = 0; n < JointNum; ++n)
		{
			::ReadFile(hf, &JointId, sizeof(JointId), &now_pos, NULL);
			now_pos += sizeof(JointId);
			JointList.push_back(JointId);
		}
		vecIsJoint.push_back(JointList);
	}

	::ReadFile(hf, &dpt, sizeof(dpt), &now_pos, NULL);
	now_pos += sizeof(dpt);
	::ReadFile(hf, &angle, sizeof(angle), &now_pos, NULL);
	now_pos += sizeof(angle);

	::ReadFile(hf, &alpha, sizeof(alpha), &now_pos, NULL);
	now_pos += sizeof(alpha);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

void TElement::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, GetElementTypeName().c_str());
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle).c_str());
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_LINE_WIDTH, this, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_COLOR_HEX, this, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("Alpha"), CTRLTYPE_INT_EDIT, &alpha, TEXT("%d"), alpha);

	//pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	//pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), RAD2DEG(angle));
}

void TElement::SetLineWidth(LONG width)
{
	logpenStyle.lopnWidth = logpenStyleShow.lopnWidth = { width,0 };
}

void TElement::SetColor(COLORREF cr)
{
	logpenStyle.lopnColor = logpenStyleShow.lopnColor = cr;
}

DPOINT TElement::GetAbsolutePointByIndex(int PointIndexOfElement) const
{
	return GetAbsolute(vecDpt[PointIndexOfElement], dpt, angle);
}

const DPOINT TElement::GetRelativePointByIndex(int PointIndexOfElement) const
{
	return vecDpt[PointIndexOfElement];
}

DPOINT GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle)
{
	return{ Org.x + dpt.x*cos(angle) - dpt.y*sin(angle),
		Org.y + dpt.x*sin(angle) + dpt.y*cos(angle) };
}

void TElement::SetX(double x)
{
	dpt.x = x;
}

void TElement::SetY(double y)
{
	dpt.y = y;
}

void TElement::SetPhi(double phi)
{
	angle = phi;
}

void TElement::ChangePos(DPOINT dptDelta)
{
	dpt += dptDelta;
}

void RegularRect(RECT &rect)
{
	if (rect.left > rect.right) std::swap(rect.left, rect.right);
	if (rect.top > rect.bottom) std::swap(rect.top, rect.bottom);
}

void TElement::SetStateNormal()
{
	bDrawSquare = false;
	logpenStyleShow=logpenStyle;
}

void TElement::SetStateHover()
{
	logpenStyleShow.lopnStyle = PS_DOT;
	logpenStyleShow.lopnColor = TDraw::GetBrighterColor(logpenStyle.lopnColor);
}

void TElement::SetStateUnHover()
{
	logpenStyleShow.lopnStyle = logpenStyle.lopnStyle;
	logpenStyleShow.lopnColor = logpenStyle.lopnColor;
}

void TElement::SetStateChosen()
{
	bDrawSquare = true;
	logpenStyleShow.lopnStyle = logpenStyle.lopnStyle;
	logpenStyleShow.lopnColor = TDraw::GetBrighterColor(logpenStyle.lopnColor);
}

bool TElement::InSelect(RECT rect, bool bSelCross, const TConfiguration *pConfig)
{
	return bSelCross ? InSelCross(rect, pConfig) : InSelWindow(rect, pConfig);
}

bool RectInRect(const RECT &rcBig, const RECT &rcSmall)
{
	POINT pt1 = { rcSmall.left, rcSmall.top }, pt2 = { rcSmall.right, rcSmall.bottom };
	return PtInRect(&rcBig, pt1) && PtInRect(&rcBig, pt2);
}

bool RectCrossRect(const RECT &rcBig, const RECT &rcSmall)
{
	POINT pt1 = { rcSmall.left, rcSmall.top },
		pt2 = { rcSmall.right, rcSmall.bottom },
		pt3 = { rcSmall.left, rcSmall.bottom },
		pt4 = { rcSmall.right, rcSmall.top };
	return PtInRect(&rcBig, pt1) || PtInRect(&rcBig, pt2) || PtInRect(&rcBig, pt3) || PtInRect(&rcBig, pt4);
}

bool APointInRect(const RECT &rect, POINT *apt, int count)
{
	for (int i = 0; i < count; ++i)
		if (!PtInRect(&rect, apt[i]))
			return false;
	return true;
}

bool APointCrossRect(const RECT &rect, POINT *apt, int count)
{
	for (int i = 0; i < count; ++i)
		if (PtInRect(&rect, apt[i]))
			return true;
	return false;
}

bool VecPointInRect(const RECT &rect, std::vector<POINT> &vecpt)
{
	for (auto &pt:vecpt)
		if (!PtInRect(&rect, pt))
			return false;
	return true;
}

bool VecPointCrossRect(const RECT &rect, std::vector<POINT> &vecpt)
{
	for (auto &pt : vecpt)
		if (PtInRect(&rect, pt))
			return true;
	return false;
}

bool RefreshDOF(TElement *pElement, int &nb, int &iCoincideNum, int &iDriverNum, int &iFrameNum, bool isAdd)
{
	switch (pElement->eClass)
	{
	case ELEMENT_CLASS_CONSTRAINT:
		isAdd?++iCoincideNum:--iCoincideNum;
		return true;
	case ELEMENT_CLASS_DRIVER:
		isAdd ? ++iDriverNum : --iDriverNum;
		return true;
	case ELEMENT_CLASS_FRAME:
		isAdd ? ++iFrameNum : --iFrameNum;
		return true;
	case ELEMENT_CLASS_NORMAL:
		isAdd ? ++nb : --nb;
		return true;
	case ELEMENT_CLASS_NONE:
	default:
		assert(0);
		return false;
	}
}

bool WriteFileString(HANDLE hf, const std::String &s, DWORD &now_pos)
{
	size_t sSize = s.length() + 1;
	::WriteFile(hf, &(sSize), sizeof(sSize), &now_pos, NULL);
	now_pos += sizeof(sSize);
	if (IsErrorShowMsgBox(TEXT("写入字符串长度")))
		return false;

	::WriteFile(hf, (s.c_str()), sSize*sizeof(TCHAR), &now_pos, NULL);
	now_pos += sSize;
	if (IsErrorShowMsgBox(TEXT("写入字符串")))
		return false;

	return true;
}

bool ReadFileString(HANDLE hf, std::String &s, DWORD &now_pos)
{
	//bool b;
	int i;

	size_t sSize=0;
	::ReadFile(hf, &(sSize), sizeof(sSize), &now_pos, NULL);
	if (now_pos == 0)return false;
	if (GetLastError() != 0) return false;
	now_pos += sizeof(sSize);

	//TCHAR *temp=NULL;
	//temp = new TCHAR[sSize];
	std::unique_ptr<TCHAR> temp(new TCHAR[sSize]);
	::ReadFile(hf, temp.get(), sSize*sizeof(TCHAR), &now_pos, NULL);
	if (now_pos == 0)return false;
	if (GetLastError() != 0) return false;
	now_pos += sSize;
	s = temp.get();
	//delete[] temp;
	return true;
}