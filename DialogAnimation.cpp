#pragma once
#include "DetectMemoryLeak.h"
#include <memory>
#include <Windows.h>
#include <process.h>
#include <algorithm>
#pragma comment(lib,"winmm.lib")//timeGetTime

#include "DialogAnimation.h"

#include "FileFunction.h"

extern "C"{
#include "gifenc\gifenc.h"
}

#include "TDraw.h"
#include "TImage.h"
#include "TTransfer.h"
#include "ShowMessage.h"

#include "resource.h"
#include "TEdit.h"
#include "TTrackbar.h"
#include "TSolver.h"
#include "TCanvas.h"
#include "TShape.h"
#include "TListBox.h"
#include "TManageTool.h"
#include "TSelectTool.h"
#include "TGraph.h"
#include "TButton.h"
#include "TCheckBox.h"

#include "TMainWindow.h"
extern TMainWindow win;

namespace DialogAnimation
{
	double time_start, time_end, time_now;
	double fps;//帧率
	int frame_start, frame_end, frame_now;
	bool isPlaying;
	bool isAnalyzing;
	bool thread_is_running;
	bool player_enable;//和播放器启用状态同步

	std::vector<TListBoxItem> vecItemsLeft, vecItemsRight;

	std::vector<double *> vecpValue;//用于拖动时改变数据
	std::vector<std::vector<double>> vecSeries;

	TCHAR szPlay[8], szPause[8];
	TButton ButtonRun;
	TButton ButtonPlay;
	TButton ButtonFirstFrame;
	TButton ButtonLastFrame;
	TButton ButtonNextFrame;
	TButton ButtonPrevFrame;
	TButton ButtonMesureP;
	TButton ButtonMesureV;
	TButton ButtonMesureA;
	TButton ButtonMesureRemove;
	TButton ButtonShowGraph;
	TCheckBox CheckBoxLoop;
	TEdit EditTimeStart;
	TEdit EditTimeEnd;
	TEdit EditFPS;
	TEdit EditTime;
	TEdit EditFrame;
	TListBox ListBoxLeft, ListBoxRight;
	TTrackbar TrackbarTime;
	TSolver *pSolver;
	TCanvas *pCanvas;
	TShape *pShape;
	TManageTool *pManageTool;
	TGraph *pGraph;

	RECT rectDlg;
	HWND hDlg;
	HINSTANCE hInst;

	static HFONT hFont;

	int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, long lParam)
	{
		//if (_tcscpy(lpelf->elfLogFont.lfFaceName, (TCHAR *)lParam) == 0)
		return 10;
		//else
		//return 0;
	}

	BOOL CALLBACK DlgAnimationProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
		{
			hInst = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
			pSolver = (win.pSolver);
			pCanvas = &(win.Canvas);
			pShape = &(win.m_Shape);
			pManageTool = &(win.m_ManageTool);
			pGraph = NULL;

			pGraph = new TGraph(&(win.m_Configuration));
			pGraph->bShowMouseLine = true;
			pGraph->SetMargin(40);
			pGraph->CreateEx(0, TEXT("图表"), TEXT("图表"),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				win.m_hWnd, (HMENU)LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_GRAPH)), (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE));
			pGraph->SetDoubleBuffer(true);
			pGraph->ShowWindow(SW_HIDE);
			pGraph->UpdateWindow();

			pGraph->sLabelX = TEXT("time (sec)");

			GetClientRect(hDlg, &rectDlg);
			DialogAnimation::hDlg = hDlg;

			//链接Player按钮
			ButtonRun.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_RUN));
			ButtonPlay.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_PLAY));
			ButtonFirstFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_FIRST_FRAME));
			ButtonLastFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_LAST_FRAME));
			ButtonNextFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_NEXT_FRAME));
			ButtonPrevFrame.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_PREV_FRAME));

			ButtonMesureP.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_MESURE_P));
			ButtonMesureV.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_MESURE_V));
			ButtonMesureA.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_MESURE_A));
			ButtonMesureRemove.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_REMOVE_MESURE));
			ButtonShowGraph.LinkControl(GetDlgItem(hDlg, IDC_BUTTON_SHOW_GRAPH));
			ButtonShowGraph.SetEnable(false);

			CheckBoxLoop.LinkControl(GetDlgItem(hDlg, IDC_CHECK_LOOP));

			//链接list
			ListBoxLeft.LinkControl(GetDlgItem(hDlg, IDC_LIST_LEFT));
			ListBoxRight.LinkControl(GetDlgItem(hDlg, IDC_LIST_RIGHT));

			InitialListBoxLeft();

			//设置Player按钮字体
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			TCHAR szFontName[] = TEXT("Segoe UI symbol1");//Segoe UI symbol1
			lf.lfCharSet = DEFAULT_CHARSET;
			HDC hdc = GetDC(hDlg);
			LONG lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			lf.lfHeight = lfHeight;
			lf.lfWeight = 700;
			_tcscpy(lf.lfFaceName, szFontName);

			int iRet = EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontFamProc, 0, 0);

			if (iRet == 10)
			{
				hFont = CreateFontIndirect(&lf);
				//hFont = CreateFont(lfHeight, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Segoe UI symbol"));

				_tcscpy(szPlay, TEXT("⏵"));
				_tcscpy(szPause, TEXT("⏸"));
				ButtonPlay.SetFont(hFont);
				ButtonFirstFrame.SetFont(hFont);
				ButtonLastFrame.SetFont(hFont);
				ButtonNextFrame.SetFont(hFont);
				ButtonPrevFrame.SetFont(hFont);
			}
			else
			{
				hFont = NULL;
				_tcscpy(szPlay, TEXT(">"));
				_tcscpy(szPause, TEXT("||"));
				ButtonPlay.SetText(TEXT(">"));
				ButtonFirstFrame.SetText(TEXT("|<"));
				ButtonLastFrame.SetText(TEXT(">|"));
				ButtonNextFrame.SetText(TEXT(">>"));
				ButtonPrevFrame.SetText(TEXT("<<"));
			}

			isPlaying = false;
			isAnalyzing = false;
			thread_is_running = false;
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
			if ((HWND)lParam == TrackbarTime.m_hWnd)//人为变更Trackbar
			{
				frame_now = TrackbarTime.GetPos();
				SetEditTimeAndEditFrame();
				InvalidateRect(hDlg, &rectDlg, FALSE);
				UpdateWindow(hDlg);

				if (!vecSeries.empty())
					for (size_t i = 0; i < vecpValue.size(); ++i)
					{
						*(vecpValue[i]) = vecSeries[frame_now][i];
					}

				for (auto pElement : pShape->Element)//刷新线的起始点
				{
					switch (pElement->eType)
					{
					case ELEMENT_REALLINE:
					case ELEMENT_BAR:
					case ELEMENT_SLIDEWAY:
						pElement->SetPhi(pElement->angle);
						break;
					}
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
			case IDC_BUTTON_RUN://WM_COMMAND
			{

				if (isAnalyzing == false)//启动分析
				{

					isAnalyzing = true;
					_beginthread(AnalyzeProc, 0, NULL);

				}
				else
				{
					//停止分析
					isAnalyzing = false;

					//AnalyzeProc感知

					//ButtonRun.SetText(TEXT("开始分析"));
				}
				//SetPlayerEnable(true);

				break;
			}
			case IDC_BUTTON_PLAY://WM_COMMAND
			{
				if (isPlaying)
				{
					ButtonPlay.SetText(szPlay);
					//->暂停
					isPlaying = false;

					//播放线程将感知到
					//禁用分析按钮
					SetAnalyzeButtonEnable(true);

					SetPlayerEnable(true);//按钮解禁
					SetMesureControlEnable(true);
				}
				else
				{
					ButtonPlay.SetText(szPause);
					//->播放
					isPlaying = true;

					//开始播放

					//禁用全部按钮，除了暂停键

					//禁用分析按钮
					SetAnalyzeButtonEnable(false);
					SetPlayerEnable(false);//禁用全部
					ButtonPlay.SetEnable(true);
					SetMesureControlEnable(false);

					//启动播放线程
					_beginthread(PlayProc, 0, NULL);
				}
				break;
			}
			case IDC_BUTTON_FIRST_FRAME://WM_COMMAND
				frame_now = frame_start;
				TrackbarTime.SetPos(frame_now);
				SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);
				break;
			case IDC_BUTTON_LAST_FRAME://WM_COMMAND
				frame_now = frame_end;
				TrackbarTime.SetPos(frame_now);
				SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);
				break;
			case IDC_BUTTON_NEXT_FRAME://WM_COMMAND
				if (frame_now < frame_end) frame_now++;
				TrackbarTime.SetPos(frame_now);
				SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);
				break;
			case IDC_BUTTON_PREV_FRAME://WM_COMMAND
				if (frame_now > 0) frame_now--;
				TrackbarTime.SetPos(frame_now);
				SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);
				break;
			case IDC_LIST_LEFT://WM_COMMAND
				switch (HIWORD(wParam))
				{
				case LBN_SELCHANGE://左列表框内容变化
				{
					int ItemIndex = ListBoxLeft.GetCurSel();
					if (ItemIndex != -1)
					{
						int id = vecItemsLeft[ItemIndex].id;//取得当前id
						int IndexOfPoint = vecItemsLeft[ListBoxLeft.GetCurSel()].index_of_point;//取得当前点号

						if (pManageTool->m_uiCurActiveTool != ID_SELECT && pManageTool->m_uiCurActiveTool != ID_DRAG)
						{
							pManageTool->SetCurActiveTool(ID_SELECT);//切换为选择工具
						}
						((TSelectTool *)(pManageTool->m_pCurrentTool))->SelectById(id, true, true);//选中id

						pCanvas->SetShowIdAndIndex(id, IndexOfPoint);
					}
					break;
				}
				case LBN_KILLFOCUS:
				{
					pCanvas->SetShowIdAndIndex(-1, -1);
					break;
				}
				}
				break;
			case IDC_BUTTON_MESURE_P:
			{
				int ItemIndex = ListBoxLeft.GetCurSel();
				if (ItemIndex != -1)
				{
					enumListBoxItemType type = D;
					for (auto Item : vecItemsRight)
						if (Item.id == vecItemsLeft[ItemIndex].id &&
							Item.index_of_point == vecItemsLeft[ItemIndex].index_of_point &&
							Item.type == type &&
							Item.value_type == vecItemsLeft[ItemIndex].value_type)//has been had
							return TRUE;

					vecItemsRight.push_back(vecItemsLeft[ItemIndex]);
					vecItemsRight.back().type = type;
					vecItemsRight.back().s = GetTypeName(type) + TEXT(": ") + vecItemsRight.back().s;
					vecItemsRight.back().sUnit = GetUnitName(type, vecItemsRight.back().value_type);

					ListBoxRight.AddString(vecItemsRight.back().s.c_str());
				}
				break;
			}
			case IDC_BUTTON_MESURE_V:
			{
				int ItemIndex = ListBoxLeft.GetCurSel();
				if (ItemIndex != -1)
				{
					enumListBoxItemType type = V;
					for (auto Item : vecItemsRight)
						if (Item.id == vecItemsLeft[ItemIndex].id &&
							Item.index_of_point == vecItemsLeft[ItemIndex].index_of_point &&
							Item.type == type &&
							Item.value_type == vecItemsLeft[ItemIndex].value_type)//has been had
							return TRUE;

					vecItemsRight.push_back(vecItemsLeft[ItemIndex]);
					vecItemsRight.back().type = type;
					vecItemsRight.back().s = GetTypeName(type) + TEXT(": ") + vecItemsRight.back().s;
					vecItemsRight.back().sUnit = GetUnitName(type, vecItemsRight.back().value_type);

					ListBoxRight.AddString(vecItemsRight.back().s.c_str());
				}
				break;
			}
			case IDC_BUTTON_MESURE_A:
			{
				int ItemIndex = ListBoxLeft.GetCurSel();
				if (ItemIndex != -1)
				{
					enumListBoxItemType type = A;
					for (auto Item : vecItemsRight)
						if (Item.id == vecItemsLeft[ItemIndex].id &&
							Item.index_of_point == vecItemsLeft[ItemIndex].index_of_point &&
							Item.type == type &&
							Item.value_type == vecItemsLeft[ItemIndex].value_type)//has been had
							return TRUE;

					vecItemsRight.push_back(vecItemsLeft[ItemIndex]);
					vecItemsRight.back().type = type;
					vecItemsRight.back().s = GetTypeName(type) + TEXT(": ") + vecItemsRight.back().s;
					vecItemsRight.back().sUnit = GetUnitName(type, vecItemsRight.back().value_type);

					ListBoxRight.AddString(vecItemsRight.back().s.c_str());
				}
				break;
			}
			case IDC_BUTTON_REMOVE_MESURE:
			{
				int ItemIndex = ListBoxRight.GetCurSel();
				if (ItemIndex != -1)
				{
					auto iter = vecItemsRight.begin() + ItemIndex;
					vecItemsRight.erase(iter);

					ListBoxRight.DeleteCurSel();
				}
				break;
			}
			case IDC_BUTTON_SHOW_GRAPH:
			{
				if (pGraph != NULL && !vecItemsRight.empty())
				{
					pGraph->bRealClose = false;
					pGraph->ShowWindow(SW_SHOWNORMAL);
				}
				break;
			}
			case ID_SAVE_AS_GIF:
			{
				uint8_t *palette = NULL;
				try
				{
					if (player_enable==false)
						throw TEXT("请先分析机构！");
					TCHAR filename[MAX_PATH];

					if (SaveFileDialog(hDlg, filename, TEXT("GIF\0*.gif\0\0"), TEXT("gif")))
					{
						DialogAnimation::SetAnalyzeButtonEnable(false);
						DialogAnimation::SetMesureControlEnable(false);
						DialogAnimation::SetPlayerEnable(false);

						int w = pCanvas->ClientRect.right;
						int h = pCanvas->ClientRect.bottom;

						int depth=8, color_num=256;

#ifdef _DEBUG
						::AllocConsole();
						freopen("CONOUT$", "w+t", stdout);
#endif

						//TDraw::GetGifPaletteByHwnd(pCanvas->m_hWnd, palette, color_num, depth);
						TDraw::Create8TreePal(pCanvas->m_hWnd, palette, color_num, depth);

						std::vector<unsigned int> vecPalette;
						unsigned int temp;
						for (int i = 0; i < color_num; ++i)
						{
							temp = (palette[i * 3 + 2] << 16) | (palette[i * 3 + 1] << 8) | palette[i * 3];
							vecPalette.push_back(temp);
						}

						std::sort(vecPalette.begin(), vecPalette.end());

						for (int i = 0; i < color_num; ++i)
						{
							palette[i * 3+2] = (unsigned char)(vecPalette[i]>>16);
							palette[i * 3+1] = (unsigned char)(vecPalette[i]>>8);
							palette[i * 3] = (unsigned char)(vecPalette[i]);
							std::cout << vecPalette[i] << std::endl;
						}

						ge_GIF *gif = ge_new_gif(
							TTransfer::unicode2string(filename).c_str(),  /* file name */
							w, h,           /* canvas size */
							palette,
							depth,              /* palette depth == log2(# of colors) */
							0               /* infinite loop */
							);

						for (frame_now = frame_start; frame_now <= frame_end; ++frame_now)
						{
							//刷新渲染
							TrackbarTime.SetPos(frame_now);
							SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);

							//得到图像并遍历
							TImage Image(pCanvas->m_hWnd);
							for (int i = 0; i < Image.height*Image.width; ++i)
							{
								UINT32 *data = (UINT32 *)Image.pvBits + i;
								*data &= 0x00FFFFFF;
								//gif->frame[i] = TDraw::GetIndexFromPalette(*data, palette, color_num);
								gif->frame[i] = TDraw::GetIndexFromPalette_Slow(*data,vecPalette, color_num);
								std::cout << "%" << double(i) / (Image.height*Image.width) << std::endl;
							}

							//加入帧
							ge_add_frame(gif, 100 / fps);
						}


						ge_close_gif(gif);
						ShowMessage(TEXT("\"%s\" 保存完成。"),filename);
					}
				}
				catch (TCHAR s[])
				{
					ShowMessage(s);
					//ShowMessage(TTransfer::char2wstring(err.what()).c_str());
				}
				catch (...)
				{
					ShowMessage(TEXT("保存出错。"));
				}

				DialogAnimation::SetAnalyzeButtonEnable(true);
				DialogAnimation::SetMesureControlEnable(true);
				DialogAnimation::SetPlayerEnable(true);

						::FreeConsole();

				if (palette != NULL)
					delete[] palette;

				//_beginthread(SaveGifProc, 0, NULL);
				break;
			}
			}
			return TRUE;
		case WM_CLOSE:
			if (thread_is_running)
			{
				isPlaying = false;
				isAnalyzing = false;

				_beginthread(PrepareClose, 0, NULL);
			}
			else
			{
				pGraph->bRealClose = true;
				if (pGraph != NULL)
					delete pGraph;

				DeleteObject(hFont);
				EndDialog(hDlg, 0);

			}
			return TRUE;
		}
		return FALSE;
	}

	VOID SaveGifProc(PVOID pvoid)
	{

		int w = pCanvas->ClientRect.right;
		int h = pCanvas->ClientRect.bottom;

		uint8_t *palette = NULL;

		int depth, color_num;
		TDraw::GetGifPaletteByHwnd(pCanvas->m_hWnd, palette, color_num, depth);

		ge_GIF *gif = ge_new_gif(
			"example.gif",  /* file name */
			w, h,           /* canvas size */
			palette,
			depth,              /* palette depth == log2(# of colors) */
			0               /* infinite loop */
			);

		for (frame_now = frame_start; frame_now <= frame_end; ++frame_now)
		{
			//刷新渲染
			TrackbarTime.SetPos(frame_now);
			SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);

			TImage Image(pCanvas->m_hWnd);
			for (int i = 0; i < Image.height*Image.width; ++i)
			{
				UINT32 *data = (UINT32 *)Image.pvBits + i;
				gif->frame[i] = TDraw::GetIndexFromPalette(*data, palette, color_num);
			}
			ge_add_frame(gif, 100 / fps);
		}

		ge_close_gif(gif);

		delete[] palette;
	}

	VOID PrepareClose(PVOID pvoid)
	{
		while (thread_is_running)
			continue;

		SendMessage(hDlg, WM_CLOSE, 0, 0);
	}

	void SetPlayerEnable(bool bEnable)
	{
		player_enable = bEnable;
		ButtonPlay.SetEnable(bEnable);
		ButtonFirstFrame.SetEnable(bEnable);
		ButtonLastFrame.SetEnable(bEnable);
		ButtonNextFrame.SetEnable(bEnable);
		ButtonPrevFrame.SetEnable(bEnable);

		EditTime.SetEnable(bEnable);
		EditFrame.SetEnable(bEnable);

		TrackbarTime.SetEnable(bEnable);
	}

	void SetMesureControlEnable(bool bEnable)
	{
		ListBoxLeft.SetEnable(bEnable);
		ListBoxRight.SetEnable(bEnable);

		ButtonMesureP.SetEnable(bEnable);
		ButtonMesureV.SetEnable(bEnable);
		ButtonMesureA.SetEnable(bEnable);
		ButtonMesureRemove.SetEnable(bEnable);
	}

	void SetAnalyzeButtonEnable(bool bEnable)
	{
		EditTimeStart.SetEnable(bEnable);
		EditTimeEnd.SetEnable(bEnable);
		EditFPS.SetEnable(bEnable);
		ButtonRun.SetEnable(bEnable);
	}

	void SetPlayerButtonEnableWhenDragTrackbar()
	{
		if (frame_now == frame_start)
		{
			ButtonFirstFrame.SetEnable(false);
			ButtonPrevFrame.SetEnable(false);
		}
		else
		{
			ButtonFirstFrame.SetEnable(true);
			ButtonPrevFrame.SetEnable(true);
		}

		if (frame_now == frame_end)
		{
			ButtonLastFrame.SetEnable(false);
			ButtonNextFrame.SetEnable(false);
		}
		else
		{
			ButtonLastFrame.SetEnable(true);
			ButtonNextFrame.SetEnable(true);
		}
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

	VOID AnalyzeProc(PVOID pvoid)
	{
		thread_is_running = true;

		ButtonRun.SetText(TEXT("中止分析"));
		SetMesureControlEnable(false);//禁用测量框

		//禁用播放器
		SetPlayerEnable(false);

		//禁用时间帧率修改
		EditTimeStart.SetEnable(false);
		EditTimeEnd.SetEnable(false);
		EditFPS.SetEnable(false);

		//取得时间诸元
		time_start = EditTimeStart.GetDouble();
		time_end = EditTimeEnd.GetDouble();
		fps = EditFPS.GetDouble();
		frame_start = time_start*fps;
		frame_end = time_end*fps;

		TrackbarTime.SetRange(frame_end);

		double spf = 1 / fps;

		std::vector<double> vecSingle;//本次位移数据

		pSolver->RefreshEquations();

		//准备记录坐标数据
		vecSeries.clear();
		pSolver->LinkpValue(vecpValue);

		//预设容量
		vecSeries.reserve(frame_end);

		//替换机架点坐标
		pSolver->SubsFramePoint();

		for (auto &Item : vecItemsRight)
			Item.data.clear();

		//链接测量数据
		std::vector<int> vecIndexOfVarTable;
		pSolver->LinkMesureResult(vecItemsRight, vecIndexOfVarTable);

		std::vector<double> vect;

		pSolver->ClearOutput();

		bool hasSolved;
		pSolver->Solve(time_start - spf);//调试发现每次的第一帧加速度计算都不正常，但后面正常。无法排除bug，先减一帧计算
		for (time_now = time_start, frame_now = frame_start; time_now <= time_end; time_now += spf, frame_now++)
		{
			//求解
			if ((hasSolved = pSolver->Solve(time_now)) && isAnalyzing == true)
			{
				//时间数组 x
				vect.push_back(time_now);

				//设置界面
				TrackbarTime.SetPos(frame_now);//走时间轴
				SetEditTimeAndEditFrame();//时间及当前帧
				InvalidateRect(hDlg, &rectDlg, FALSE);//刷新界面
				UpdateWindow(hDlg);//刷新界面

				//刷新画布
				pCanvas->Invalidate();
				UpdateWindow(pCanvas->m_hWnd);

				//保存坐标数据
				pSolver->GetResult(vecSingle);
				vecSingle.pop_back();//the last one is "t"
				vecSeries.push_back(vecSingle);//存入一组坐标数据

				//保存测量数据
				pSolver->GetMesureResult(vecItemsRight, vecIndexOfVarTable);//存入待测位移、速度、加速度
			}
			else
			{
				if (hasSolved == false)
				{
					ShowMessage(TEXT("求解失败。\r\n\r\n可能的原因：到达极限位置；存在多余的约束或驱动。"));
				}
				time_now -= spf;
				frame_now--;

				//停止分析

				//设置截止时间为当前时间
				time_end = time_now;
				frame_end = frame_now;
				EditTimeEnd.SetDouble(time_end);
				TrackbarTime.SetRange(frame_end);

				//设置界面
				TrackbarTime.SetPos(frame_now);
				SetEditTimeAndEditFrame();
				InvalidateRect(hDlg, &rectDlg, FALSE);
				UpdateWindow(hDlg);

				break;
			}
		}

		//DialogAnimation::vecItemsRight;

		//加载曲线数据
		pGraph->bDraw = false;
		pGraph->Clear();
		for (auto &Item : vecItemsRight)
		{
			LOGPEN logpen;
			switch (Item.type)
			{
			case D:
				logpen = { PS_SOLID, { 1, 0 }, RGB(0, 0, 0) }; break;
			case V:
				logpen = { PS_DASH, { 1, 0 }, RGB(0, 200, 0) }; break;
			case A:
				logpen = { PS_DASHDOT, { 1, 0 }, RGB(200, 0, 0) }; break;
			}
			pGraph->InputDptVector(vect, Item.data, logpen, true, Item.s.c_str(), Item.sUnit.c_str());

		}

		pGraph->bDraw = true;
		pGraph->Refresh();

		ButtonShowGraph.SetEnable(!vecItemsRight.empty());//有测量数据则显示图表按钮可用

		SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_SHOW_GRAPH, 0), 0);//按下显示图表按钮

		//启用播放器
		SetPlayerEnable(true);

		//启用时间帧率修改
		EditTimeStart.SetEnable(true);
		EditTimeEnd.SetEnable(true);
		EditFPS.SetEnable(true);

		//回到第一帧
		SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_FIRST_FRAME, 0), 0);

		pShape->SimplifyPhiValue();

		isAnalyzing = false;
		ButtonRun.SetText(TEXT("开始分析"));
		SetMesureControlEnable(true);//解禁

		thread_is_running = false;
	}

	VOID PlayProc(PVOID pvoud)
	{
		thread_is_running = true;

		DWORD start = timeGetTime();//当前真实时间

		if (frame_now == frame_end) frame_now = frame_start;
		int frame_start_this_time = frame_now;//本轮播放的起始帧

		DWORD now;
		int frame_prev = frame_now;
		while (frame_now < frame_end)
		{
			if (isPlaying == false)
			{
				thread_is_running = false;
				return;
			}

			now = timeGetTime();
			frame_now = (now - start) / 1000.0*fps + frame_start_this_time;//得到当前应处理的帧
			if (frame_now == frame_prev)//和上一次处理相同，则不刷新
				continue;

			//刷新渲染
			TrackbarTime.SetPos(frame_now);
			SendMessage(hDlg, WM_HSCROLL, 0, (LPARAM)TrackbarTime.m_hWnd);

			//OutputDebugInt(frame_now);

			frame_prev = frame_now;
		}

		if (CheckBoxLoop.GetChecked())//循环按钮启用
		{
			isPlaying = false;
			SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_FIRST_FRAME, 0), 0);
			SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_PLAY, 0), 0);
		}
		else
			//放完自动停止
			SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_BUTTON_PLAY, 0), 0);//再点一次播放按钮即停止

		//_endthread();
		//return 0;
		thread_is_running = false;
	}

	//初始化左列表
	void InitialListBoxLeft()
	{
		vecItemsLeft.clear();
		vecItemsRight.clear();

		String s, s_dpt;
		for (auto pElement : pShape->Element)//遍历所有元素
		{
			if (pElement->CanBeDragged())//非机架及RealLine,Driver,Constraint
			{
				s.clear();
				s << TEXT("ID:") << pElement->id;
				s << TEXT(" ") << pElement->GetElementTypeName();//“连杆”
				s << TEXT(" ") << pElement->Name;
				for (size_t i = 0; i < pElement->vecDpt.size(); ++i)//遍历每个相对点
				{
					for (auto ValueType : { X, Y, PHI })//x,y,phi各加一个
					{
						if (i != 0 && ValueType == PHI) continue;//只有第一个点有phi参数

						TListBoxItem tempItem;

						s_dpt.clear();
						s_dpt << TEXT("pt") << i;
						switch (ValueType)
						{
						case X:s_dpt << TEXT(" x");  break;
						case Y:s_dpt << TEXT(" y");  break;
						case PHI:s_dpt << TEXT(" phi");  break;
						}
						ListBoxLeft.AddString((s + s_dpt).c_str());

						tempItem.id = pElement->id;
						tempItem.index_of_point = i;
						tempItem.s = s + s_dpt;
						tempItem.pElement = pElement;

						tempItem.value_type = ValueType;
						vecItemsLeft.push_back(tempItem);
					}
				}
			}
		}
	}

	String GetTypeName(enumListBoxItemType type)
	{
		switch (type)
		{
		case D:return TEXT("位移"); break;
		case V:return TEXT("速度"); break;
		case A:return TEXT("加速度"); break;
		default:
			return TEXT("error"); break;
		}
	}

	String GetUnitName(enumListBoxItemType type, enumListBoxItemValueType value_type)
	{
		String s = GetTypeName(type);
		s << TEXT(" (");
		switch (value_type)
		{
		case X:s << TEXT("mm"); break;
		case Y:s << TEXT("mm"); break;
		case PHI:s << TEXT("rad"); break;
		}

		switch (type)
		{
		case D:s << TEXT(")"); break;
		case V:s << TEXT("/s)"); break;
		case A:s << TEXT("/s^2)"); break;
		}
		return s;
	}
}