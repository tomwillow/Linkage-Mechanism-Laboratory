#include "TAttach.h"


TAttach::TAttach()
{
}


TAttach::~TAttach()
{
}

void TAttach::Load(HWND hCanvas, TShape *shape, TConfiguration *config)
{
	TAttach::hCanvas = hCanvas;
	TAttach::Shape = shape;
	TAttach::Config = config;
}

void TAttach::Draw(HDC hdc)
{
	if (bAttachLine)
	{
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}
}

void TAttach::AttachAll(POINT ptPos)
{
	DPOINT ptDPos = Config->ScreenToReal(ptPos);
	AttachLine(ptDPos);
}

void TAttach::AttachLine(DPOINT ptDPos)
{
	bAttachLine = false;
	for (int i = 0; i < Shape->uiLineNum; i++)
	{
		//Îü¸½Æðµã
		if (abs(Config->LengthToScreenX(ptDPos.x - Shape->RealLine[i].ptBegin.x)) < 10 &&
			abs(Config->LengthToScreenY(ptDPos.y - Shape->RealLine[i].ptBegin.y)) < 10)
		{
			bAttachLine = true;
			dptAttach = Shape->RealLine[i].ptBegin;
			break;
		}

		//Îü¸½ÖÕµã
		if (abs(Config->LengthToScreenX(ptDPos.x - Shape->RealLine[i].ptEnd.x)) < 10 &&
			abs(Config->LengthToScreenY(ptDPos.y - Shape->RealLine[i].ptEnd.y)) < 10)
		{
			bAttachLine = true;
			dptAttach = Shape->RealLine[i].ptEnd;
			break;
		}
	}
}