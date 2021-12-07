#pragma once
#include "TLineTool.h"

#include "..\Control\TCanvas.h"
#include "..\resource.h"
#include "..\Common\TCHAR_Function.h"
#include "..\Control\TTreeViewContent.h"

#include "..\TConfiguration.h"
#include "..\Draw\TDraw.h"
#include "..\Control\TLineEdit.h"
#include "..\Tool\TAttach.h"
#include "..\Common\TTransfer.h"
#include "..\Solver\TSolver.h"

#include "..\Element\TLine.h"
#include "..\Element\TRealLine.h"
#include "..\Element\TConstraintCoincide.h"

TLineTool::TLineTool()
{
	Attach = new TAttach(pCanvas, pShape, pConfig);
	bShowDimLine = false;
	MoveLine = new TRealLine;
	MoveLine->SetStyle(pConfig);

	Line1 = new TLine;
	Line2 = new TLine;
	LineDim = new TLine;
	Line1->SetStyle(pConfig->logpenAssist);
	Line2->SetStyle(pConfig->logpenAssist);
	LineDim->SetStyle(pConfig->logpenAssist);

	LineEdit = new TLineEdit;

	CoincideBegin = NULL;

	myElementType = ELEMENT_REALLINE;

	pPrevLine = NULL;

	bShowTips = true;
	sType = TEXT("�߶�");
	bCanBuildCoincide = false;
}


TLineTool::~TLineTool()
{
	DestroyWindow(LineEdit->m_hWnd);
	delete Attach;

	delete LineEdit;
	delete MoveLine;

	delete Line1;
	delete Line2;
	delete LineDim;

	if (CoincideBegin != NULL)
		delete CoincideBegin;
}

void TLineTool::OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)
{
	OnMouseMove(hWnd, nFlags, ptPos);
}

void TLineTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//bShowDptText = false;
	Attach->AttachAll(ptPos, MoveLine->ptBegin);
	//MoveLine->ptEnd = Attach->dptAttach;
	MoveLine->SetPoint(MoveLine->ptBegin, Attach->dptAttach);

	if (dptHit.size() > 0)
	{
		//�����ݴ��߳��Ȳ�����Edit
		//MoveLine->CalcLength();
		LineEdit->SetText(TEXT("%.3f"), MoveLine->dLength);
		SetFocus(LineEdit->m_hWnd);
		LineEdit->SetSelectAll();

		//����ߴ��߸�������
		int dist = 40;//�ݴ��ߺͳߴ��߾���
		double theta = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);
		pt1 = pConfig->RealToScreen(MoveLine->ptBegin);
		pt2 = pConfig->RealToScreen(MoveLine->ptEnd);
		Line1->ptBegin = pt1;
		Line2->ptBegin = pt2;
		if (MoveLine->ptEnd.y - MoveLine->ptBegin.y >= 0)//λ��1,2����
		{
			TDraw::Move(&pt1, 1, theta + M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta + M_PI / 2, dist);
		}
		else
		{
			TDraw::Move(&pt1, 1, theta - M_PI / 2, dist);
			TDraw::Move(&pt2, 1, theta - M_PI / 2, dist);
		}
		Line1->ptEnd = pt1;
		Line2->ptEnd = pt2;
		LineDim->ptBegin = pt1; LineDim->ptEnd = pt2;
		//���ߴ��߳���Ϊ0����ʾ
		if (LineDim->ptBegin.x == LineDim->ptEnd.x && LineDim->ptBegin.y == LineDim->ptEnd.y)
			bShowDimLine = false;
		else
			bShowDimLine = true;

		//����Editλ�ò���ʾ
		if (LineEdit->m_hWnd != NULL && bShowDimLine == true)
		{
			int width = 60, height = 20;
			POINT ptMiddle;
			ptMiddle.x = (LineDim->ptEnd.x + LineDim->ptBegin.x) / 2;
			ptMiddle.y = (LineDim->ptEnd.y + LineDim->ptBegin.y) / 2;

			LineEdit->SetPosition(ptMiddle.x - width / 2, ptMiddle.y - height / 2, width, height);
			LineEdit->SetVisible(true);
		}
		else
			LineEdit->SetVisible(false);
	}

	ptMouse = pConfig->RealToScreen(Attach->dptAttach);
	if (ptMouse.x == ptPrevPos.x && ptMouse.y == ptPrevPos.y)
		sTips = TEXT("����Ϊ0:���ƶ�������������");
	else
	{
		sTips = TEXT("����Խ���");
		sTips += sType;
		if (bShowDimLine)
			sTips += TEXT("\r\n�������ʽ: ���� or ����<�Ƕ� or x����,y���꣨��Ե�ǰ�㣩��");
		if (Attach->bAttachedEndpoint && bCanBuildCoincide)
			sTips += TEXT("\r\n�������˵�:�Զ������غ�Լ��");
	}

	//��Canvasˢ��
}

//�����Ȳ���WM_PAINT�¼��н��л���
void TLineTool::Draw(HDC hdc)
{

	Attach->Draw(hdc);//������Attach�ṩ

	//����ʱ�߼��ߴ���
	if (dptHit.size() > 0)
	{
		switch (myElementType)
		{
		case ELEMENT_REALLINE://���һ�κ�ſ�ʼ����ʱ��
			TDraw::DrawRealLine(hdc, *MoveLine, pConfig);
			break;
		case ELEMENT_BAR:
			TDraw::DrawBar(hdc, (TBar *)MoveLine, pConfig);
			break;
		case ELEMENT_SLIDEWAY:
			TDraw::DrawSlideway(hdc, (TSlideway *)MoveLine, pConfig);
			break;
		}

		if (bShowDimLine)
		{
			TDraw::DrawLine(hdc, *Line1);
			TDraw::DrawLine(hdc, *Line2);
			TDraw::DrawLine(hdc, *LineDim);
		}

	}

	if (bShowTips)
		TDraw::DrawTips(hdc, ptMouse, ClientRect, sTips.c_str(), pConfig);
}

TElement * TLineTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(0));
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(1));

	RealLine.eType = myElementType;
	return pShape->AddElement(&RealLine);
}

void TLineTool::AddIntoTreeViewContent(TElement *Element, int id)
{
	//Լ��ҲҪ�����˴�
	if (Element->eType == ELEMENT_REALLINE)
		Element->eType = myElementType;
	pTreeViewContent->AddItem(Element, pShape->iNextId);
}

//LineEdit����ʱCanvas�ǽ��ղ���KEYDOWN�ģ���ȻLineToolҲ���ղ�����
//KEYDOWN��Ϣ��LineEdit�ػ��͸�Canvas���ٴ���������
void TLineTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		OnRButtonDown(hWnd, 0, { 0, 0 });
		break;
	case VK_SPACE:
	case VK_RETURN:
		if (LineEdit->GetVisible())
		{
			TRealLine RealLine;
			RealLine = *MoveLine;

			//���
			AddIntoTreeViewContent(&RealLine, pShape->iNextId);
			pPrevLine = AddIntoShape(RealLine);

			if (CoincideBegin != NULL)
			{
				//��һ��Լ�����
				CoincideBegin->pElement[1] = pPrevLine;

				AddCoincide(CoincideBegin, pShape->iNextId, pConfig);

				pSolver->RefreshEquations();

				delete CoincideBegin;
				CoincideBegin = NULL;
			}

			//��������Լ����ID��һ��.end=ID���.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);

			CoincideBegin->pElement[0] = pPrevLine;
			CoincideBegin->PointIndexOfElement[0] = 1;//ptEnd

			CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin

			//����ó����յ�����ݴ�㼯
			dptHit.push_back(RealLine.ptEnd);

			//������ʱ��
			POINT ptNew = pConfig->RealToScreen(RealLine.ptEnd);
			InitialLine(RealLine.ptEnd);
			Attach->InitialLine(ptNew);

			LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

			//�ƶ���������
			TDraw::ClientPosToScreen(hWnd, &ptNew);
			SetCursorPos(ptNew.x, ptNew.y);
		}
		break;
	default:
		//���м������˴�����ֹ��Edit�ڵ�

		//�õ��ַ���ת��
		double length, DegAngle;
		DPOINT dpt;
		TCHAR buffer[64];
		LineEdit->GetText(buffer);

		std::vector<TCHAR *>szNums;
		TCHAR_Function::Split(buffer, szNums, TEXT(","));//�з�����

		enum enumInputType{ INPUT_BAD, INPUT_LENGTH, INPUT_COORDINATE, INPUT_LENGTH_ANGLE } eInputType;
		if (szNums.size() == 2)//ȷʵ������
		{
			eInputType = INPUT_COORDINATE;
			dpt.x = TTransfer::TCHAR2double(szNums[0]);
			dpt.y = TTransfer::TCHAR2double(szNums[1]);
		}
		else
		{
			szNums.clear();
			TCHAR_Function::Split(buffer, szNums, TEXT("<"));
			length = TTransfer::TCHAR2double(szNums[0]);
			if (abs(length) > 1e-6)//����>0
				if (szNums.size() == 2)
				{
					eInputType = INPUT_LENGTH_ANGLE;
					DegAngle = TTransfer::TCHAR2double(szNums[1]);
				}
				else
				{
					eInputType = INPUT_LENGTH;
				}
			else
				eInputType = INPUT_BAD;
		}
		TCHAR_Function::ReleaseVectorTCHAR(szNums);
		//�ѵõ�dpt,length,angle

		switch (eInputType)
		{
		case INPUT_COORDINATE:
			MoveLine->ptEnd.x = MoveLine->ptBegin.x + dpt.x;
			MoveLine->ptEnd.y = MoveLine->ptBegin.y + dpt.y;
			break;
		case INPUT_LENGTH:
			DegAngle = TDraw::GetAngleFromPointReal(MoveLine->ptBegin, MoveLine->ptEnd);//����ֵΪ����
			MoveLine->SetPointByIvoryLine(Attach->GetiIvoryLine(), MoveLine->ptBegin, length, DegAngle);
			break;
		case INPUT_LENGTH_ANGLE:
			MoveLine->SetPointByDegAngle(MoveLine->ptBegin, length, DegAngle);
			break;
		}
		//bShowDptText = true;
		//dptText = MoveLine->ptEnd;

		Attach->dptAttach = MoveLine->ptEnd;

		pCanvas->Invalidate();

		break;
	}

}

void TLineTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)
{
	if (bCanBuildCoincide)
	{
		AddIntoTreeViewContent(pCoincide, id);
		pShape->AddElement(pCoincide);
	}
}


void TLineTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//bShowDptText = false;
	ptPos = pConfig->RealToScreen(MoveLine->ptEnd);

	//����Լ��
	if (dptHit.size() == 0)//��һ�㲶׽����Լ��
	{
		if (Attach->bAttachedEndpoint)
		{
			//ID����׽=ID��Ԫ��.begin
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);
			CoincideBegin->pElement[0] = Attach->pAttachElement;
			CoincideBegin->PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

			CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin
		}
	}
	else//�ǵ�һ��
	{
		if (ptPos.x == ptPrevPos.x && ptPos.y == ptPrevPos.y)
			return;

		//������1���������
		TRealLine RealLine;
		RealLine.SetStyle(MoveLine->logpenStyle);//ֻ�д˴�����⣬����MoveLine��ɫ��MoveLine��ɫȡ��
		MoveLine->SetStyle(pConfig);
		RealLine.SetPoint(dptHit[dptHit.size() - 1], MoveLine->ptEnd);

		//iPrevLineId = pShape->iNextId;
		AddIntoTreeViewContent(&RealLine, pShape->iNextId);
		pPrevLine = AddIntoShape(RealLine);

		if (CoincideBegin != NULL)
		{
			//��һ��Լ�����
			CoincideBegin->pElement[1] = pPrevLine;

			AddCoincide(CoincideBegin, pShape->iNextId, pConfig);
			//AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			//pShape->AddCoincide(*CoincideBegin, pConfig);

			pSolver->RefreshEquations();
			//RefreshTreeViewContent();
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		if (Attach->bAttachedEndpoint)//���յ㲶׽����
		{
			//ID����׽=ID���.end
			CoincideBegin = new TConstraintCoincide;
			CoincideBegin->SetStyle(pConfig->logpen);
			//CoincideBegin->eElementType1 = Attach->eAttachElementType;
			CoincideBegin->pElement[0] = Attach->pAttachElement;
			CoincideBegin->PointIndexOfElement[0] = Attach->iAttachElementPointIndex;

			//CoincideBegin->eElementType2 = myElementType;
			CoincideBegin->pElement[1] = pPrevLine;//�˴�����⣬�����Ԫ��idΪnextid-1
			CoincideBegin->PointIndexOfElement[1] = 1;//ptEnd

			//�յ��������
			AddCoincide(CoincideBegin, pShape->iNextId, pConfig);
			//AddIntoTreeViewContent(CoincideBegin, pShape->iNextId);
			//pShape->AddCoincide(*CoincideBegin, pConfig);

			pSolver->RefreshEquations();

			delete CoincideBegin;
			CoincideBegin = NULL;
		}


		//��������Լ����ID��һ��.end=ID���.begin
		CoincideBegin = new TConstraintCoincide;
		CoincideBegin->SetStyle(pConfig->logpen);
		//CoincideBegin->eElementType1 = myElementType;
		CoincideBegin->pElement[0] = pPrevLine;//��һ����id
		CoincideBegin->PointIndexOfElement[0] = 1;//ptEnd

		//CoincideBegin->eElementType2 = myElementType;
		//CoincideBegin->ElementId2 = pShape->iNextId;//�˴�δ��⣬��Ϊ��ǰ���
		CoincideBegin->PointIndexOfElement[1] = 0;//ptBegin
	}

	//��ǰ������ݴ�㼯
	dptHit.push_back(MoveLine->ptEnd);

	//������ʱ��
	InitialLine(MoveLine->ptEnd);
	Attach->InitialLine(ptPos);

	//����LineEdit������
	if (LineEdit->m_hWnd == NULL)
	{
		LineEdit->CreateEditEx(pCanvas->m_hWnd, IDR_LINEEDIT, pCanvas->m_hInst);
	}
	else
		LineEdit->SetVisible(false);//�����ڻ����ߺ���ʾEdit

	ptPrevPos = ptPos;
	//��������ˢ��
}

void TLineTool::InitialLine(DPOINT dptPos)
{
	POINT ptPos = pConfig->RealToScreen(dptPos);
	MoveLine->ptBegin = dptPos;
	MoveLine->ptEnd = dptPos;

	Line1->ptBegin = ptPos;
	Line1->ptEnd = ptPos;
	Line2->ptBegin = ptPos;
	Line2->ptEnd = ptPos;
	LineDim->ptBegin = ptPos;
	LineDim->ptEnd = ptPos;
	bShowDimLine = true;
}

void TLineTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//bShowDptText = false;
	if (dptHit.size() == 0)//û�е������µ��Ҽ������ù���
		::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
	else
	{
		//���Ѵ���Edit������Edit
		if (LineEdit->m_hWnd != NULL)
			LineEdit->SetVisible(false);

		bShowDimLine = false;

		//�����ݴ��غ�Լ��
		if (CoincideBegin != NULL)
		{
			delete CoincideBegin;
			CoincideBegin = NULL;
		}

		dptHit.clear();

		pCanvas->Invalidate();

	}
}

void TLineTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}