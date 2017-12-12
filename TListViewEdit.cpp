#pragma once
#include "DetectMemoryLeak.h"
#include "tchar_head.h"
#include "MyMath.h"
#include <vector>

#include "TListViewEdit.h"
#include "TCHAR_Function.h"
#include "TTransfer.h"
#include "DPOINT.h"

#include "TElement.h"
#include "TRealLine.h"
TListViewEdit::TListViewEdit()
{
	ListItemIndex = -1;
}


TListViewEdit::~TListViewEdit()
{
}

void TListViewEdit::ChangeSource()
{
	switch (eCtrlType)
	{
	case CTRLTYPE_EDIT:
		this->GetText(pElement->Name);
		
		break;
	case CTRLTYPE_COOR_EDIT:
	case CTRLTYPE_COOR_P1_EDIT:
	case CTRLTYPE_COOR_P2_EDIT:
	{
		this->GetText();

		DPOINT dpt;
		std::vector<TCHAR *>szNums;
		TCHAR_Function::Split(Text, szNums, TEXT(","));//切分坐标

		if (szNums.size() == 2)//确实是坐标
		{
			dpt.x = TTransfer::TCHAR2double(szNums[0]);
			dpt.y = TTransfer::TCHAR2double(szNums[1]);
			switch (eCtrlType)
			{
			case CTRLTYPE_COOR_EDIT:
				*((DPOINT *)pContent) = dpt;
				break;
			case CTRLTYPE_COOR_P1_EDIT:
			{
				TRealLine *pRealLine = (TRealLine *)pElement;
				pRealLine->SetPoint(dpt, pRealLine->ptEnd);
				break;
			}
			case CTRLTYPE_COOR_P2_EDIT:
			{
				TRealLine *pRealLine = (TRealLine *)pElement;
				pRealLine->SetPoint(pRealLine->ptBegin, dpt);
				break;
			}
			}
			//length,angle
		}
		else
		{
			MessageBeep(1);
		}
		TCHAR_Function::ReleaseVectorTCHAR(szNums);
		break;
	}
	case CTRLTYPE_DOUBLE_EDIT:
	{
		this->GetText();

		double value = TTransfer::TCHAR2double(Text);
		*((double *)pContent) = value;
		break;
	}
	case CTRLTYPE_INT_EDIT:
	{
		this->GetText();

		*((int *)pContent) = TTransfer::TCHAR2int(Text);
		break;
	}
	case CTRLTYPE_ANGLE_VALUE_EDIT:
	{
		this->GetText();

		double DegAngle = TTransfer::TCHAR2double(Text);
		*((double *)pContent) = DEG2RAD(DegAngle);
		break;
	}
	case CTRLTYPE_COLOR_HEX:
	{
		this->GetText();

		pElement->SetColor(_tcstol(Text, NULL, 16));
		break;
	}
	case CTRLTYPE_LINE_WIDTH:
	{
		this->GetText();

		pElement->SetLineWidth(TTransfer::TCHAR2int(Text));
		break;
	}
	case CTRLTYPE_LEN_EDIT:
	{
		this->GetText();

		double len = TTransfer::TCHAR2double(Text);
		TRealLine *pRealLine = (TRealLine *)pElement;
		pRealLine->SetPointByDegAngle(pRealLine->ptBegin, len, RAD2DEG(pRealLine->dAngle));
		break;
	}
	case CTRLTYPE_ANGLE_EDIT:
	{
		this->GetText();

		double DegAngle = TTransfer::TCHAR2double(Text);
		TRealLine *pRealLine = (TRealLine *)pElement;
		pRealLine->SetPointByDegAngle(pRealLine->ptBegin, pRealLine->dLength, DegAngle);
		break;
	}
	default:
		assert(0);
		break;
	}
}

bool TListViewEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_RETURN)
	{
		ChangeSource();
		this->SetVisible(false);
		
		PostMessage(m_hParent, WM_USER, 0, 0);//通知ListView更新

		return false;
	}
	return true;
}

bool TListViewEdit::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	ChangeSource();
	this->SetVisible(false);
	PostMessage(m_hParent, WM_USER, 0, 0);//通知ListView更新
	return true;
}