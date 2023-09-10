#pragma once
#include "TStatus.h"


TStatus::TStatus()
{
	m_hWnd = NULL;
	m_ControlID = 0;
	m_iPartsNum = 0;
	m_piPartsWidth = NULL;
	m_pePartsType = NULL;
	m_piID = NULL;
	m_piPartsRightPos = NULL;

	flex_num = 0;
	has_used_width = 0;
}


TStatus::~TStatus()
{
	free(m_piPartsWidth);
	free(m_piPartsRightPos);
	free(m_pePartsType);
	free(m_piID);
}


void TStatus::Create(HWND hParent, UINT nControlID,HINSTANCE hInst,int iHeight)
{
	m_ControlID = nControlID;
	m_hWnd = CreateWindowEx(0,STATUSCLASSNAME, TEXT("statusbar"), WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, 0, 0, 0, 0, hParent, (HMENU)nControlID, hInst, 0);

	RECT rectParent;
	::GetClientRect(hParent, &rectParent);
	::MoveWindow(m_hWnd, 0, rectParent.bottom - iHeight, rectParent.right, iHeight, TRUE);
	m_iHeight = iHeight;
	m_hParent = hParent;

	RegisterProc();
}

RECT TStatus::GetClientRect()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	return rect;
}

void TStatus::FreshSize()
{
	RECT rectParent;
	::GetClientRect(m_hParent, &rectParent);
	::MoveWindow(m_hWnd, 0, rectParent.bottom - m_iHeight, rectParent.right, m_iHeight, TRUE);

	int remaid_width = rectParent.right - has_used_width;
	int now_x = 0;
	for (UINT i = 0; i < m_iPartsNum; i++)
	{
		//将空余宽度均分给非固定块
		if (m_pePartsType[i] == PT_NONE)
		{
			m_piPartsWidth[i] = remaid_width / flex_num;
		}
		m_prcPartsRect[i].left = now_x;
		m_prcPartsRect[i].right = m_piPartsWidth[i];
		m_prcPartsRect[i].top = 0;
		m_prcPartsRect[i].bottom =TStatus::GetClientRect().bottom;
		now_x += m_piPartsWidth[i];
		m_piPartsRightPos[i] = now_x;
	}

	SendMessage(m_hWnd, SB_SETPARTS, m_iPartsNum, (LPARAM)m_piPartsRightPos);
}


void TStatus::SetText(int pane, LPCTSTR str)
{
	SendMessage(m_hWnd, SB_SETTEXT, pane, (LPARAM)str);
}

void CDECL TStatus::SetText(int id, TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	for (UINT i = 0; i < m_iPartsNum; i++)
	{
		if (id == m_piID[i])
		{
			SendMessage(m_hWnd, SB_SETTEXT, i, (LPARAM)szBuffer);
			break;
		}
	}
}

//得到某块的Rect，保存的是坐标
RECT TStatus::GetPartRect(int id, int margin)
{
	for (UINT i = 0; i < m_iPartsNum; i++)
	{
		if (id == m_piID[i])
		{
			RECT rect = m_prcPartsRect[i];
			rect.top += margin;
			rect.left += margin;
			rect.bottom -= margin * 2;
			rect.right -= margin * 2;
			return rect;
		}
	}
	throw(id);
}

void TStatus::AddPart(int id,int iWidth,PartType type)
{
	m_iPartsNum++;
	m_piPartsWidth = (int *)realloc(m_piPartsWidth, m_iPartsNum*sizeof(int));
	m_piPartsRightPos = (int *)realloc(m_piPartsRightPos, m_iPartsNum*sizeof(int));
	m_pePartsType = (PartType *)realloc(m_pePartsType, m_iPartsNum*sizeof(PartType));
	m_piID = (int *)realloc(m_piID, m_iPartsNum*sizeof(int));
	m_prcPartsRect = (RECT *)realloc(m_prcPartsRect, m_iPartsNum*sizeof(RECT));
	m_piPartsWidth[m_iPartsNum - 1] = iWidth;
	m_pePartsType[m_iPartsNum - 1] = type;
	m_piID[m_iPartsNum - 1] = id;

	switch (type)
	{
	case PT_FIXED:
		has_used_width += iWidth;
		break;
	case PT_NONE:
		flex_num++;
		break;
	}
}
