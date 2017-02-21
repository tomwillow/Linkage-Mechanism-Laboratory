#pragma once
#include "MyMath.h"
#include "DetectMemoryLeak.h"
#include <vector>

#include "TListViewEdit.h"
#include "TMyString.h"
#include "TTransfer.h"
#include "DPOINT.h"

#include "TElement.h"
#include "TRealLine.h"
TListViewEdit::TListViewEdit()
{
	ListItemIndex = -1;
	Text = NULL;
}


TListViewEdit::~TListViewEdit()
{
	if (Text != NULL)
		delete[] Text;
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
		this->GetText(Text);

		DPOINT dpt;
		std::vector<TCHAR *>szNums;
		TMyString::Split(Text, szNums, TEXT(","));//切分坐标

		if (szNums.size() == 2)//确实是坐标
		{
			dpt.x = TTransfer::TCHAR2double(szNums[0]);
			dpt.y = TTransfer::TCHAR2double(szNums[1]);
			switch (eCtrlType)
			{
			case CTRLTYPE_COOR_EDIT:
				pElement->dpt = dpt;
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
		TMyString::ReleaseVectorTCHAR(szNums);
		break;
	}
	case CTRLTYPE_LEN_EDIT:
	{
		this->GetText(Text);

		double len = TTransfer::TCHAR2double(Text);
		TRealLine *pRealLine = (TRealLine *)pElement;
		pRealLine->SetPointByDegAngle(pRealLine->ptBegin, len, REG2DEG(pRealLine->dAngle));
		break;
	}
	case CTRLTYPE_ANGLE_EDIT:
	{
		this->GetText(Text);

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
		
		PostMessage(hParent, WM_USER, 0, 0);//通知ListView更新

		return false;
	}
	return true;
}

bool TListViewEdit::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	ChangeSource();
	this->SetVisible(false);
	PostMessage(hParent, WM_USER, 0, 0);//通知ListView更新
	return true;
}

void CDECL TListViewEdit::SetText(TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	::SetWindowText(m_hWnd, szBuffer);

	Text = new TCHAR[sizeof(TCHAR)*(GetLength() + 1)];
	_tcscpy(Text, szBuffer);

}


void TListViewEdit::GetText(TCHAR text[])
{
	Text = new TCHAR[sizeof(TCHAR)*(GetLength() + 1)];
	::GetWindowText(m_hWnd, Text, GetLength() + 1);//不知道为什么要加1才取得全

	_tcscpy(text, Text);
}