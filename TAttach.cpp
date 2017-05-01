#pragma once
#include "DetectMemoryLeak.h"

#include "TCanvas.h"
#include "TAttach.h"
#include "TSlider.h"

#include "TFramePoint.h"
#include "TRealLine.h"
#include "TPolylineBar.h"

TAttach::TAttach(TCanvas *pCanvas, TShape *pShape, TConfiguration *pConfig)
{
	TAttach::pCanvas = pCanvas;
	TAttach::pShape = pShape;
	TAttach::pConfig = pConfig;

	iIvoryLine = 0;

	//设置极轴线
	bShowXAssist = false;
	bShowYAssist = false;

	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	XAssistLine->SetStyle(pConfig->logpenAssistLine);
	YAssistLine->SetStyle(pConfig->logpenAssistLine);

	//设置延长线
	bShowExtensionLine = false;
	ExtensionLine = new TRealLine;
	ExtensionLine->SetStyle(pConfig->logpenAssistLine);

	//设置捕捉点
	bAttachedEndpoint = true;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	iAttachElementPointIndex = -1;

	bAttachedEndpointSelf = false;
}


TAttach::~TAttach()
{
	delete XAssistLine;
	delete YAssistLine;

	delete ExtensionLine;
}

void TAttach::InitialLine(POINT ptPos)
{
	XAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	XAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
}

void TAttach::Draw(HDC hdc)
{

	if (bShowAttachPoint)
	{
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}
	//画X辅助线
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	//画Y辅助线
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	if (bShowExtensionLine)
	{
		TDraw::DrawRealLine(hdc, *ExtensionLine, pConfig);//
	}

}

void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);

	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴
	AttachAxis(dptAttach, dptCheckPos);

//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）

	//越靠上越优先
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
				return;
}

void TAttach::AttachAll(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴

//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）

	//越靠上越优先
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
			return;
}

//以绝对坐标vecdptAbsolute顺序构成的线段，检测ptNowPos是否位于线上，并设置参数：
//bAttachedEnepoint
//bShowAttachPoint
//bShowExtensionLine  ExtensionLine
//iAttachLinePointIndex[0] iAttachLinePointIndex[1]
bool TAttach::AttachLine_Element(DPOINT dptNowPos,const std::vector<DPOINT> vecdptAbsolute)
{
	if (vecdptAbsolute.empty()) return false;

	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pt = vecdptAbsolute.begin(); pt != vecdptAbsolute.end() - 1; ++pt)
	{
		int status = TDraw::PointInRealLineOrExtension(dptNowPos,dptAttach, *pt, *(pt + 1), pConfig);
		if (status == -1)
			continue;

		//已捕捉
		bAttachedEndpoint = false;
		bShowAttachPoint = true;
		bShowExtensionLine = true;

		iAttachLinePointIndex[0] = pt - vecdptAbsolute.begin();
		iAttachLinePointIndex[1] = pt + 1 - vecdptAbsolute.begin();

		switch (status)
		{
		case 1://-1都不在 0在线段上 1在pt1一侧延长线 2在pt2一侧延长线
			ExtensionLine->SetPoint(*pt, dptAttach);
			return true;
		case 2:
			ExtensionLine->SetPoint(*(pt + 1), dptAttach);
			return true;
		case 0:
			ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
			return true;
		}
	}
	return false;
}

void TAttach::AttachLine(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachLine_inner(dptAttach);
}

bool TAttach::AttachLine_inner(DPOINT dptNowPos)
{
	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pElement:pShape->Element)
	{
		switch (pElement->eType)
		{
		case ELEMENT_SLIDER:
		case ELEMENT_POLYLINEBAR:
		{
			std::vector<DPOINT> vecdptAbsolute;
			TDraw::GetAbsoluteReal(vecdptAbsolute, pElement->vecDpt, pElement->dpt, pElement->angle);

			if (AttachLine_Element(dptNowPos, vecdptAbsolute))
			{
				pAttachElement = pElement;
				return true;
			}

			//吸附Slider
			if (pElement->eType == ELEMENT_SLIDER)
			{
				std::vector<DPOINT> vecdptAbsoluteSlider;
				vecdptAbsoluteSlider.push_back(pElement->GetAbsolutePointByIndex(-1));
				vecdptAbsoluteSlider.push_back(pElement->GetAbsolutePointByIndex(0));

				if (AttachLine_Element(dptNowPos, vecdptAbsoluteSlider))
				{
					iAttachLinePointIndex[0] = -1;
					iAttachLinePointIndex[1] = 0;
					pAttachElement = pElement;
					return true;
				}
			}

			break;
		}
		case ELEMENT_REALLINE:
		case ELEMENT_BAR:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)pElement;
			int status = TDraw::PointInRealLineOrExtension(dptNowPos, dptAttach, pRealLine->ptBegin,pRealLine->ptEnd, pConfig);
			if (status == -1)
				break;

				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				pAttachElement = pElement;

				iAttachLinePointIndex[0] = 0;
				iAttachLinePointIndex[1] = 1;

			switch (status)
			{
			case 1://-1都不在 0在线段上 1在pt1一侧延长线 2在pt2一侧延长线
				ExtensionLine->SetPoint(((TRealLine *)pElement)->ptBegin, dptAttach);
				return true;
			case 2:
				ExtensionLine->SetPoint(((TRealLine *)pElement)->ptEnd, dptAttach);
				return true;
			case 0:
				ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
				return true;
			}
		}
		case ELEMENT_FRAMEPOINT:
		case CONSTRAINT_COINCIDE:
		case CONSTRAINT_COLINEAR:
			break;
		default:
			assert(0);
			break;
		}
	}
	return false;
}

bool TAttach::AttachLineSelf(DPOINT dptNowPos)
{
	if (AttachLine_Element(dptNowPos, vecdpt))
	{
		//pAttachElement = pElement;
		return true;
	}
	else
		return false;
}

//检查NowPos是否靠近以CheckPos为原点的极轴，使用前应设置dptAttach为当前点
bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos)
{

	iIvoryLine = 0;
	RECT CanvasClientRect;
	::GetClientRect(pCanvas->m_hWnd, &CanvasClientRect);
	//检测X轴辅助线
	XAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenY(dptNowPos.y - dptCheckPos.y)) < 10)//当前点和起始点y轴偏差10像素
	{
		bShowXAssist = true;

		//设置辅助线x,y坐标
		if (dptNowPos.x - XAssistLine->ptBegin.x > 0)//当前位置位于辅助线起始点右边
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.right);
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.left);
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		//dptAttach.x = dptNowPos.x;
		dptAttach.y = XAssistLine->ptEnd.y;//将当前点吸附到坐标轴
	}
	else
	{
		bShowXAssist = false;
	}

	//检测Y轴辅助线
	YAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenX(dptNowPos.x - dptCheckPos.x)) < 10)
	{
		bShowYAssist = true;
		if (dptNowPos.y - YAssistLine->ptBegin.y > 0)//当前位置位于辅助线起始点上边
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.top);
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.bottom);
			iIvoryLine = 4;
		}
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;

		//dptAttach.y = dptNowPos.y;
		dptAttach.x = YAssistLine->ptEnd.x;//吸附
	}
	else
	{
		bShowYAssist = false;
	}

	if (bShowXAssist || bShowYAssist)
		return true;
	else
		return false;
}

//读取pShape中的RealLine进行吸附，使用前应设置dptAttach为当前点
bool TAttach::AttachPoint(DPOINT dptPos)
{
	bAttachedEndpoint = false;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	for (auto pElement: pShape->Element)
	{
		eAttachElementType = pElement->eType;
		switch (pElement->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)pElement;
			//吸附起点
			if (DPTisApproached(dptPos, pRealLine->ptBegin))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 0;
				dptAttach = pRealLine->ptBegin;
				return true;
			}

			//吸附终点
			if (DPTisApproached(dptPos, pRealLine->ptEnd))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 1;
				dptAttach = pRealLine->ptEnd;
				return true;
			}
			break;
		}
		case ELEMENT_FRAMEPOINT:
		{
			TFramePoint *pFramePoint = (TFramePoint *)pElement;
			//吸附机架点
			if (DPTisApproached(dptPos, pFramePoint->dpt))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pFramePoint;
				iAttachElementPointIndex = 0;
				dptAttach = pFramePoint->dpt;
				return true;
			}
			break;
		}
		case ELEMENT_POLYLINEBAR:
		case ELEMENT_SLIDER:
		{
			for (auto iter = pElement->vecDpt.begin(); iter != pElement->vecDpt.end(); ++iter)
			{
				if (DPTisApproached(dptPos, TDraw::GetAbsolute(*iter, pElement->dpt, pElement->angle)))
				{
					bAttachedEndpoint = true;
					bShowAttachPoint = true;
					pAttachElement = pElement;
					iAttachElementPointIndex = iter - pElement->vecDpt.begin();
					dptAttach = TDraw::GetAbsolute(*iter, pElement->dpt, pElement->angle);
					return true;
				}
			}
			break;
		}
		//不吸附端点的元素
		case CONSTRAINT_COLINEAR:
		case CONSTRAINT_COINCIDE:
			break;
		default:
			assert(0);
			break;
		}
	}


	return false;
}

//吸附vecdpt，使用前应设置dptAttach为当前点
bool TAttach::AttachPointSelf(DPOINT dptPos)
{
	bAttachedEndpointSelf = false;
	bShowAttachPoint = false;
	for (auto dpt : vecdpt)
	{
		if (DPTisApproached(dptPos, dpt))
		{
			bAttachedEndpointSelf = true;
			bShowAttachPoint = true;
			dptAttach = dpt;
			return true;
		}
	}
	return false;
}

//两点屏幕距离小于distance
bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2)
{
	if (abs(pConfig->LengthToScreenX(dpt1.x - dpt2.x)) < iAttachPixel &&
		abs(pConfig->LengthToScreenY(dpt1.y - dpt2.y)) < iAttachPixel)
		return true;
	else
		return false;
}