#pragma once
#include "DetectMemoryLeak.h"
#include "MyMath.h"
#include <stdio.h>

#include "TConfiguration.h"
#include "ShowMessage.h"
#include "TElement.h"

#include "TListView.h"
TElement::TElement() :available(true)
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

void TElement::BuildpDpt()
{ 
	assert(0); 
}

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

TCHAR * TElement::GetElementTypeName(TCHAR name[])
{
	switch (eType)
	{
	case ELEMENT_REALLINE:
		_tcscpy(name, TEXT("线"));
		break;
	case ELEMENT_FRAMEPOINT:
		_tcscpy(name, TEXT("机架"));
		break;
	case ELEMENT_BAR:
		_tcscpy(name, TEXT("连杆"));
		break;
	case ELEMENT_SLIDEWAY:
		_tcscpy(name, TEXT("滑道"));
		break;
	case ELEMENT_SLIDER:
		_tcscpy(name, TEXT("滑块"));
		break;
	case ELEMENT_POLYLINEBAR:
		_tcscpy(name, TEXT("多段杆"));
		break;
	case CONSTRAINT_COINCIDE:
		_tcscpy(name, TEXT("重合"));
		break;
	case CONSTRAINT_COLINEAR:
		_tcscpy(name, TEXT("共线"));
		break;
	default:
		assert(0);
		break;
	}
	return name;
}

TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[])
{
	switch (linestyle)
	{
	case PS_SOLID:
		_tcscpy(name, TEXT("实线"));
		break;
	case PS_DASH:
		_tcscpy(name, TEXT("线段"));
		break;
	case PS_DOT:
		_tcscpy(name, TEXT("虚线"));
		break;
	case PS_DASHDOT:
		_tcscpy(name, TEXT("点划线"));
		break;
	case PS_DASHDOTDOT:
		_tcscpy(name, TEXT("双点划线"));
		break;
	default:
		assert(0);
		break;
	}
	return name;
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
	now_pos += sizeof(tempSize);
	vecDpt.clear();
	for (size_t i = 0; i < tempSize;++i)
	{
		::ReadFile(hf, &tempDpt, sizeof(tempDpt), &now_pos, NULL);
		now_pos += sizeof(tempDpt);
		vecDpt.push_back(tempDpt);
	}

	::ReadFile(hf, &tempSize, sizeof(tempSize), &now_pos, NULL);
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

	TCHAR buffer[16];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, TEXT("机架"));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("Alpha"), CTRLTYPE_INT_EDIT, &alpha, TEXT("%d"), alpha);

	//pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	//pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));
}