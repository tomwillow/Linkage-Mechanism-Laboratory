#pragma once
#include <Windows.h>

#include "DialogAnimation.h"

#include "resource.h"
#include "TEdit.h"
#include "TTrackbar.h"
#include "TSolver.h"
#include "TCanvas.h"

#include "TMainWindow.h"
extern TMainWindow win;

namespace DialogAnimation
{
	double time_start, time_end, time_now;
	double fps;
	int frame_start, frame_end, frame_now;
	bool isPlaying;

	std::vector<double *> vecpValue;
	std::vector<std::vector<double>> vecSeries;

	TControl ButtonPlay;
	TControl ButtonFirstFrame;
	TControl ButtonLastFrame;
	TControl ButtonNextFrame;
	TControl ButtonPrevFrame;
	TEdit EditTimeStart;
	TEdit EditTimeEnd;
	TEdit EditFPS;
	TEdit EditTime;
	TEdit EditFrame;
	TTrackbar TrackbarTime;
	TSolver *pSolver;
	TCanvas *pCanvas;

	BOOL CALLBACK DlgAnimationProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static HFONT hFont;
		switch (message)
		{
		case WM_INITDIALOG:
		{
			pSolver = (win.pSolver);
			pCanvas = &(win.Canvas);

			ButtonPlay.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_PLAY));
			ButtonFirstFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_FIRST_FRAME));
			ButtonLastFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_LAST_FRAME));
			ButtonNextFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_NEXT_FRAME));
			ButtonPrevFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_PREV_FRAME));

			LONG lfHeight = -MulDiv(12, GetDeviceCaps(GetDC(ButtonPlay.m_hWnd), LOGPIXELSY), 72);
			hFont = CreateFont(lfHeight, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Segoe UI symbol"));

			ButtonPlay.SetFont(hFont);
			ButtonFirstFrame.SetFont(hFont);
			ButtonLastFrame.SetFont(hFont);
			ButtonNextFrame.SetFont(hFont);
			ButtonPrevFrame.SetFont(hFont);

			isPlaying = false;
			//COLORREF cr = RGB(0, 113, 188);

			EditTimeStart.LinkControl(GetDlgItem(hDlg, IDC_EDIT_TIME_START));
			EditTimeEnd.LinkControl(GetDlgItem(hDlg, IDC_EDIT_TIME_END));
			EditFPS.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FPS));
			EditTime.LinkControl(GetDlgItem(hDlg, IDC_EDIT_TIME));
			EditFrame.LinkControl(GetDlgItem(hDlg, IDC_EDIT_FRAME));

			TrackbarTime.LinkControl(GetDlgItem(hDlg, IDC_SLIDER_TIME));

			time_start = 0;
			time_end = 10;
			time_now = 0;
			fps = 10;
			frame_start = 0;
			frame_end = time_end*fps;
			frame_now = 0;

			EditTimeStart.SetDouble(time_start);
			EditTimeEnd.SetDouble(time_end);
			EditFPS.SetDouble(fps);
			TrackbarTime.SetRange(frame_end);
			TrackbarTime.SetPos(frame_now);

			SetEditTimeAndEditFrame();

			SetPlayerEnable(false);

			return TRUE;
		}
		//case WM_DRAWITEM:
		//{
		//	DRAWITEMSTRUCT *DrawItemStruct = (DRAWITEMSTRUCT *)lParam;
		//	ButtonPlay.DrawItem(DrawItemStruct);
		//	return 0;
		//}
		case WM_HSCROLL:
		{
			if ((HWND)lParam == TrackbarTime.m_hWnd)
			{
				frame_now = TrackbarTime.GetPos();
				SetEditTimeAndEditFrame();

				for (size_t i = 0; i < vecpValue.size();++i)
				{
					*(vecpValue[i]) = vecSeries[frame_now][i];
				}

				//刷新画布
				pCanvas->Invalidate();
				UpdateWindow(pCanvas->m_hWnd);
			}
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_RUN:
			{
				RECT rectDlg;
				GetClientRect(hDlg, &rectDlg);
				//SetPlayerEnable(true);

				time_start = EditTimeStart.GetDouble();
				time_end = EditTimeEnd.GetDouble();
				fps = EditFPS.GetDouble();
				frame_start = 0;
				frame_end = time_end*fps;

				TrackbarTime.SetRange(frame_end);

				double spf = 1 / fps;

				std::vector<double> vecSingle;

				//准备记录坐标数据
				vecSeries.clear();
				pSolver->LinkpValue(vecpValue);

				for (time_now = time_start, frame_now = frame_start; time_now <= time_end; time_now += spf, frame_now++)
				{
					pSolver->Solve(time_now);

					//设置界面
					TrackbarTime.SetPos(frame_now);
					SetEditTimeAndEditFrame();
					InvalidateRect(hDlg, &rectDlg, FALSE);
					UpdateWindow(hDlg);

					//刷新画布
					pCanvas->Invalidate();
					UpdateWindow(pCanvas->m_hWnd);

					//保存坐标数据
					pSolver->GetResult(vecSingle);
					vecSingle.pop_back();//the last one is "t"
					vecSeries.push_back(vecSingle);
				}
				SetPlayerEnable(true);
				break;
			}
			case IDC_BUTTON_PLAY:
			{
				if (isPlaying)
				{
					ButtonPlay.SetText(TEXT("⏵"));
					//->暂停
					isPlaying = false;
				}
				else
				{
					ButtonPlay.SetText(TEXT("⏸"));
					//->播放
					isPlaying = true;
				}
				break;
			}
			case IDOK:
			{
				//添加约束

			}
			case IDCANCEL:
				EndDialog(hDlg, 0);
				break;
			}
			return TRUE;
		case WM_CLOSE:
			DeleteObject(hFont);
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	}

	void SetPlayerEnable(bool bEnable)
	{
		ButtonPlay.SetEnable(bEnable);
		ButtonFirstFrame.SetEnable(bEnable);
		ButtonLastFrame.SetEnable(bEnable);
		ButtonNextFrame.SetEnable(bEnable);
		ButtonPrevFrame.SetEnable(bEnable);

		EditTime.SetEnable(bEnable);
		EditFrame.SetEnable(bEnable);

		TrackbarTime.SetEnable(bEnable);
	}

	void SetEditTimeAndEditFrame()
	{
		time_now = frame_now / fps;

		int min1 = time_now / 60;
		int sec1 = (int)time_now % 60;
		int mil1 = (time_now - (int)time_now) * 100;

		int min2 = time_end / 60;
		int sec2 = (int)time_end % 60;
		int mil2 = (time_end - (int)time_end) * 100;
		EditTime.SetText(TEXT("%02d:%02d.%02d / %02d:%02d.%02d"), min1, sec1, mil1, min2, sec2, mil2);
		EditFrame.SetText(TEXT("%d / %d"), frame_now, frame_end);
	}
}