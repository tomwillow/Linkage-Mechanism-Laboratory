#pragma once
#include "..\Common\DetectMemoryLeak.h"
#include "..\Common\MyMath.h"
#include "TRealLine.h"

#include "..\Tool\TAttach.h"
#include "..\TConfiguration.h"
#include "..\Draw\TDraw.h"
#include "..\Control\TListView.h"

TRealLine::TRealLine() :ptBegin(dpt)
{
	TRealLine::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_REALLINE;
	eClass = ELEMENT_CLASS_NORMAL;

	vecIsJoint.resize(2);
}

TRealLine::~TRealLine()
{
}

const String TRealLine::GetElementTypeName()//得到类型名称
{
	return TEXT("线");
}

void TRealLine::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	pListView->AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_P1_EDIT, &ptBegin, TEXT("%.3f,%.3f"), ptBegin.x, ptBegin.y);
	pListView->AddAttributeItem(TEXT("P1"), CTRLTYPE_COOR_P2_EDIT, &ptEnd, TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	pListView->AddAttributeItem(TEXT("长度"), CTRLTYPE_LEN_EDIT, NULL, TEXT("%f"), dLength);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_EDIT, NULL, TEXT("%f"), RAD2DEG(dAngle));
}


void TRealLine::SetPoint(DPOINT dptBegin, DPOINT dptEnd)
{
	TRealLine::ptBegin = dptBegin;
	TRealLine::ptEnd = dptEnd;
	dLength = sqrt((ptEnd.x - ptBegin.x)*(ptEnd.x - ptBegin.x) + (ptEnd.y - ptBegin.y)*(ptEnd.y - ptBegin.y));
	dAngle = TDraw::GetAngleFromPointReal(ptBegin, ptEnd);
}

void TRealLine::SetPoint(DPOINT dptBegin, double dLength, double dAngle)
{
	TRealLine::ptBegin = dptBegin;
	TRealLine::ptEnd.x = dptBegin.x + dLength*cos(dAngle);
	TRealLine::ptEnd.y = dptBegin.y + dLength*sin(dAngle);
	this->dLength = dLength;
	this->dAngle = dAngle;
}

//自动识别90度等角度
void TRealLine::SetPointByDegAngle(DPOINT dptBegin, double dLength, double dDegAngle)
{
	int iIvoryLine = 0;
	int iAngle = (int)dDegAngle;
	if (dDegAngle - iAngle < 1e-6)//整数
	{
		if (iAngle == 0)
			iIvoryLine = 1;
		else
			if (iAngle % 180 == 0)
				iIvoryLine = 3;
			else
				if ((iAngle > 0 && iAngle % 90 == 0) || (iAngle < 0 && iAngle % 270 == 0))
					iIvoryLine = 2;
				else
					if ((iAngle > 0 && iAngle % 270 == 0) || (iAngle < 0 && iAngle % 90 == 0))
						iIvoryLine = 4;
	}
	SetPointByIvoryLine(iIvoryLine, dptBegin, dLength, DEG2RAD(dDegAngle));
}

//若iIvory==0，则使用length和angle。否则按照象限数及length设置端点
void TRealLine::SetPointByIvoryLine(int iIvoryLine, DPOINT dptBegin, double length, double RegAngle)
{
	ptBegin = dptBegin;
	switch (iIvoryLine)
	{
	case 0://没在xy轴上
		SetPoint(ptBegin, length, RegAngle);
		break;
	case 1://→
		ptEnd = { ptBegin.x + length, ptBegin.y };
		dLength = length;
		dAngle = 0;
		break;
	case 3://←
		ptEnd = { ptBegin.x - length, ptBegin.y };
		dLength = length;
		dAngle = M_PI;
		break;
	case 2://↑
		ptEnd = { ptBegin.x, ptBegin.y + length };
		dLength = length;
		dAngle = M_PI / 2;
		break;
	case 4://↓
		ptEnd = { ptBegin.x, ptBegin.y - length };
		dLength = length;
		dAngle = 1.5*M_PI;
		break;
	}
}

TRealLine& TRealLine::operator=(const TRealLine &realline)
{
	TElement::operator=(realline);

	TLine::operator=(realline);

	this->ptEnd = realline.ptEnd;
	return *this;
}

bool TRealLine::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TElement::WriteFile(hf, now_pos);

	::WriteFile(hf, &ptEnd, sizeof(ptEnd), &now_pos, NULL);
	now_pos += sizeof(ptEnd);

	::WriteFile(hf, &dLength, sizeof(dLength), &now_pos, NULL);
	now_pos += sizeof(dLength);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TRealLine::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape)
{
	TElement::ReadFile(hf, now_pos,pShape);

	::ReadFile(hf, &ptEnd, sizeof(ptEnd), &now_pos, NULL);
	now_pos += sizeof(ptEnd);

	::ReadFile(hf, &dLength, sizeof(dLength), &now_pos, NULL);
	now_pos += sizeof(dLength);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

const DPOINT TRealLine::GetRelativePointByIndex(int PointIndexOfElement)const 
{
	switch (PointIndexOfElement)
	{
	case 0:
		return{ 0, 0 };
	case 1:
		return{ dLength, 0 };
	default:
		assert(0);
		break;
	}
	throw(PointIndexOfElement);
}

DPOINT TRealLine::GetAbsolutePointByIndex(int PointIndexOfElement)const 
{
	switch (PointIndexOfElement)
	{
	case 0:
		return dpt;
	case 1:
		return ptEnd;
	default:
		assert(0);
		break;
	}
	throw(PointIndexOfElement);
}

void TRealLine::SetPhi(double phi)
{
	angle = phi;
	SetPoint(ptBegin, dLength, angle);
}

void TRealLine::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawRealLine(hdc, ptBegin, ptEnd, logpenStyleShow, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

void TRealLine::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	//画拾取方格
	TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(ptBegin));
	TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(ptEnd));
}

void TRealLine::ChangePos(DPOINT dptDelta)
{
	ptBegin += dptDelta;
	ptEnd += dptDelta;
}

bool TRealLine::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInRealLine(ptPos, this, pConfig);
}

bool TRealLine::InSelWindow(RECT rect, const TConfiguration *pConfig)
{
	RegularRect(rect);
	if (PtInRect(&rect, pConfig->RealToScreen(ptBegin)) && PtInRect(&rect, pConfig->RealToScreen(ptEnd)))
		return true;
	else
		return false;
}

bool TRealLine::InSelCross(RECT rect, const TConfiguration *pConfig)
{
	RegularRect(rect);
	if (PtInRect(&rect, pConfig->RealToScreen(ptBegin)) || PtInRect(&rect, pConfig->RealToScreen(ptEnd)))
		return true;
	else
		return false;
}

bool TRealLine::IsAttached(DPOINT dptNowPos,TAttach *pAttach,const TConfiguration *pConfig)
{
	int PointIndex[2] = { 0, 1 };
	return pAttach->AttachLine_Element_inner(dptNowPos, ptBegin, ptEnd,this, PointIndex, pConfig);
};

bool TRealLine::PointIsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)
{
	if (pAttach->AttachPointByElement(dptNowPos, ptBegin, 0, this, pConfig))
		return true;
	else
		if (pAttach->AttachPointByElement(dptNowPos, ptEnd, 1, this, pConfig))
			return true;
	return false;
}