#pragma once
#include "TRealLine.h"

#include "TDraw.h"
#include "TListView.h"

TRealLine::TRealLine() :ptBegin(dpt)
{
	TRealLine::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_REALLINE;

	vecIsJoint.resize(2);
}

TRealLine::~TRealLine()
{
}

void TRealLine::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT,&Name, Name);// stringToTCHAR(&Name,buffer)
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, GetElementTypeName(buffer));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P1"), CTRLTYPE_COOR_P1_EDIT, &ptBegin, TEXT("%.3f,%.3f"), ptBegin.x, ptBegin.y);
	pListView->AddAttributeItem(TEXT("P2"), CTRLTYPE_COOR_P2_EDIT, &ptEnd, TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	pListView->AddAttributeItem(TEXT("长度"), CTRLTYPE_LEN_EDIT, NULL, TEXT("%f"), dLength);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_EDIT, NULL, TEXT("%f"), REG2DEG(dAngle));
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
	SetPointByIvoryLine(iIvoryLine, dptBegin, dLength, DEG2REG(dDegAngle));
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