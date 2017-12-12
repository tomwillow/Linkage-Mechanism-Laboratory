#pragma once
#include <vector>
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


	int iAttachPixel = 10;
	bool TAttach::AttachLine_inner(DPOINT dptNowPos);
	bool TAttach::AttachPointSelf(DPOINT dptPos);

	bool TAttach::AttachLineSelf(DPOINT dptNowPos);//与vecdpt配套
	bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos);
	bool TAttach::AttachPoint(DPOINT dptPos);
public:
	std::vector<DPOINT> vecdpt;//暂存点，保存PolylineBar之前画过的点，为绝对坐标

	int iAttachLinePointIndex[2];//吸附线段时，被吸附线的P Q点在vecDpt中的序号
	bool bShowExtensionLine;//会显示延长线，绘制ExtensionLine，作为判断是否吸附直线的依据

	bool bAttachedEndpoint;//吸附端点，为true则吸附上了确切存在的点
	bool bAttachedEndpointSelf;//吸附上了vecdpt里的端点，用于PolylineBar绘制
	//EnumElementType eAttachElementType;

	bool bShowAttachPoint;//会画叉
	TElement *pAttachElement;
	int iAttachElementPointIndex;
	DPOINT dptAttach;

	TAttach(TCanvas *pCanvas, TShape *shape, TConfiguration *config);
	~TAttach();
	void TAttach::InitialLine(POINT ptPos);
	void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos);
	void TAttach::AttachAll(POINT ptNowPos);
	void TAttach::AttachLine(POINT ptNowPos);
	void TAttach::Draw(HDC hdc);
	int TAttach::GetiIvoryLine()
	{
		return iIvoryLine;
	}

	bool TAttach::AttachPointByElement(DPOINT dptNowPos, DPOINT &dpt, int iPointIndex,const TElement *pElement, const TConfiguration *pConfig);
	bool TAttach::AttachLineByRelativeVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptRelative, const TConfiguration *pConfig);
	bool TAttach::AttachLineByAbsoluteVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptAbsolute);
	bool TAttach::AttachLine_Element_inner(DPOINT dptNowPos, DPOINT dptAbsolute1, DPOINT dptAbsolute2,const TElement *pElement, int PointIndex[2],
		const TConfiguration *pConfig);

	bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2);
};

