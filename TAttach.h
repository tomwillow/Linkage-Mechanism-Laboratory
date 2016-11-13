#pragma once
#include <Windows.h>
#include "DPOINT.h"
#include "TShape.h"
#include "TConfiguration.h"
#include "TDraw.h"

class TAttach
{
private:
	TShape *Shape;
	TConfiguration *Config;
	int iIvoryLine;
	TRealLine *XAssistLine, *YAssistLine;//X,Y÷·∏®÷˙œﬂ
	bool bShowXAssist, bShowYAssist;//œ‘ æX,Y÷·∏®÷˙œﬂ
	bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance);
public:
	DPOINT dptAttach;
	bool bAttachPoint;
	HWND hCanvas;
	TAttach(HWND hCanvas, TShape *shape, TConfiguration *config);
	~TAttach();
	void TAttach::Load(HWND hCanvas, TShape *shape, TConfiguration *config);
	void TAttach::InitialLine(POINT ptPos);
	void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachAll(POINT ptNowPos);
	bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachPoint(DPOINT dptPos);
	void TAttach::Draw(HDC hdc);
	int TAttach::GetiIvoryLine()
	{
		return iIvoryLine;
	}
};

