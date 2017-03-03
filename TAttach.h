#pragma once
#include <Windows.h>
#include "DPOINT.h"
#include "TShape.h"
#include "TConfiguration.h"
#include "TDraw.h"

class TCanvas;
class TAttach
{
private:
	//bool bAttachPoint;
	TCanvas *pCanvas;
	TShape *pShape;
	TConfiguration *pConfig;
	int iIvoryLine;
	TRealLine *XAssistLine, *YAssistLine;//X,Y轴辅助线
	bool bShowXAssist, bShowYAssist;//显示X,Y轴辅助线

	TRealLine *ExtensionLine;


	bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance);
	bool TAttach::AttachLine(POINT ptNowPos);
public:
	bool bShowExtensionLine;//已拾取直线
	bool bAttachedEndpoint;
	EnumElementType eAttachElementType;

	bool bShowAttachPoint;
	TElement *pAttachElement;
	int iAttachElementPointIndex;
	DPOINT dptAttach;
	TAttach(TCanvas *pCanvas, TShape *shape, TConfiguration *config);
	~TAttach();
	void TAttach::InitialLine(POINT ptPos);
	void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachAll(POINT ptNowPos);
	bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos);
	bool TAttach::AttachPoint(DPOINT dptPos);
	void TAttach::Draw(HDC hdc);
	int TAttach::GetiIvoryLine()
	{
		return iIvoryLine;
	}
};

