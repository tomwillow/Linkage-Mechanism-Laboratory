#pragma once
//#define _STUDENT

#include "DetectMemoryLeak.h"

#include <process.h>
#include "TMainWindow.h"

#include "TToolTip.h"

#include "String.h"
#include "TTransfer.h"
#include "ShowMessage.h"

#include "TDraw.h"

#include "TConstraintCoincide.h" 
#include "TConstraintColinear.h"
#include "TSlider.h"
#include "TSolver.h"
#include "TConsole.h"
#include "TSelectTool.h"

#include "DialogAddDriver.h"
#include "DialogOption.h"
#include "DialogAbout.h"
#include "DialogAnimation.h"

#include "global.h"
#ifdef _STUDENT
#include "DialogStudent.h"
#endif
#ifdef _TEACHER
#include "DialogScore.h"
#endif

#include "TGraph.h"

#include "FileFunction.h"

TMainWindow::TMainWindow() :Canvas(&m_Configuration,&m_ManageTool,&m_Status,&m_Trackbar,&m_Shape)
{
	m_iRightWindowWidth = 200;
	pConsole = NULL;
	pSolver = new TSolver;
}


TMainWindow::~TMainWindow()
{
	if (pConsole != NULL)
	{
		delete pConsole;
	}

	if (pSolver != NULL)
		delete pSolver;

	for (auto iter = vecpGraph.begin(); iter != vecpGraph.end(); ++iter)
	{
		delete *iter;
	}
	//_CrtDumpMemoryLeaks();
}


void TMainWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。

	//配置初始化
	this->m_Configuration.Initial(hWnd);

	//创建工具栏
	m_Toolbar.bAutoWrap = true;
	m_Toolbar.bIsFlat = true;
	//m_Toolbar.bTextOnRight = true;
	m_Toolbar.CreateToolbar(hWnd, m_hInst);
	m_Toolbar.LoadImageList(32, 32, IDR_TOOLBAR_MAIN, RGB(255, 255, 255));
	m_Toolbar.AddGroup(0, 0, ID_SELECT, true, TEXT("选择"));
	m_Toolbar.AddGroup(1, 0, ID_DRAG, true, TEXT("拖动"));
	m_Toolbar.AddButton(2, ID_REFRESH, true, TEXT("刷新"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(3, 0, ID_DRAW_FRAME, true, TEXT("机架"));
	m_Toolbar.AddGroup(7, 0, ID_DRAW_SLIDEWAY, true, TEXT("固定导轨"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(4, 0, ID_DRAW_BAR, true, TEXT("杆"));
	m_Toolbar.AddGroup(5, 0, ID_DRAW_POLYLINE_BAR, true, TEXT("多副构件"));
	m_Toolbar.AddGroup(6, 0, ID_DRAW_LINE, true, TEXT("线"));
	m_Toolbar.AddGroup(8, 0, ID_DRAW_SLIDER, true, TEXT("滑块"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddGroup(9, 0, ID_DRAW_COINCIDE, true, TEXT("重合约束"));
	m_Toolbar.AddGroup(10, 0, ID_DRAW_COLINEAR, true, TEXT("共线约束"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddButton(11, ID_SET_DRIVER, true, TEXT("设为原动件"));
	m_Toolbar.AddButton(12, ID_ANIMATION, true, TEXT("动画与测量"));
	m_Toolbar.ShowToolbar();

	//创建状态栏
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);//坐标
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_CHECKBOX_AXES, 22, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_CHECKBOX_SHOW_REAL, 22, PT_FIXED);//连杆绘制/真实绘制
	m_Status.AddPart(IDR_STATUS_CHECKBOX_THEME, 22, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);//比例：
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);//
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);//比例数字
	m_Status.AddPart(0, 6, PT_FIXED);//留给拖动三角的
	m_Status.FreshSize();

	m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："), m_Configuration.GetProportion() * 100);
	m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);

	//创建真实切换按钮
	m_CheckBoxShowReal.LoadCheckedBitmap(m_hInst, IDR_BITMAP_SHOW_REAL);
	m_CheckBoxShowReal.LoadUnCheckedBitmap(m_hInst, IDR_BITMAP_SHOW_SIMPLE);
	m_CheckBoxShowReal.CreateBitmapCheckBox(m_hInst, m_Status.m_hWnd, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 1).left, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 0).top, 20, 20, IDR_STATUS_CHECKBOX_SHOW_REAL);
	m_CheckBoxShowReal.SetCheckedAndBitmap(m_Configuration.bDrawReal);

	//创建风格切换按钮
	m_CheckBoxTheme.LoadCheckedBitmap(m_hInst, IDB_BITMAP_THEME_DARK);
	m_CheckBoxTheme.LoadUnCheckedBitmap(m_hInst, IDB_BITMAP_THEME_BRIGHT);
	m_CheckBoxTheme.CreateBitmapCheckBox(m_hInst, m_Status.m_hWnd, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 1).left, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 0).top, 20, 20, IDR_STATUS_CHECKBOX_THEME);
	m_CheckBoxTheme.SetCheckedAndBitmap(m_Configuration.bThemeDark);

	//创建坐标轴切换按钮
	m_CheckBoxAxes.LoadCheckedBitmap(m_hInst, IDB_BITMAP_AXES_SHOW);
	m_CheckBoxAxes.LoadUnCheckedBitmap(m_hInst, IDB_BITMAP_AXES_HIDE);
	m_CheckBoxAxes.CreateBitmapCheckBox(m_hInst, m_Status.m_hWnd, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_AXES, 1).left, m_Status.GetPartRect(IDR_STATUS_CHECKBOX_AXES, 0).top, 20, 20, IDR_STATUS_CHECKBOX_AXES);
	m_CheckBoxAxes.SetCheckedAndBitmap(m_Configuration.bDrawAxes);

	//创建tooltip
	CreateToolTip(m_Status.m_hWnd, m_CheckBoxShowReal.m_hWnd, m_hInst, TEXT("点击以切换 真实/简图 显示"));
	CreateToolTip(m_Status.m_hWnd, m_CheckBoxTheme.m_hWnd, m_hInst, TEXT("点击以切换 深色/浅色 背景"));
	CreateToolTip(m_Status.m_hWnd, m_CheckBoxAxes.m_hWnd, m_hInst, TEXT("点击以切换显示坐标轴"));

	//创建Trackbar
	m_Trackbar.CreateTrackbar(m_Status.m_hWnd, this->m_hInst, m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0), IDR_TRACKBAR);
	m_Trackbar.SetRangeAndValue({ 0.01, 0.05, 0.1, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16, 32, 64 });
	m_Trackbar.SetPosByValue(1.0, 1e-6);

	//创建右窗口
	RightWindow.CreateEx(0, TEXT("RightWindow"), TEXT("RightWindow"),
		WS_CHILD,
		0, 0, 0, 0,
		hWnd,
		(HMENU)ID_RIGHTWINDOW,//id
		m_hInst);
	SetRightWindowPos();
	RightWindow.ShowWindow(TRUE);
	RightWindow.UpdateWindow();

	//创建画布
	Canvas.CreateEx(0, TEXT("canvas"), TEXT("canvas"),
		WS_CHILD,
		0,
		0,
		0,
		0,
		hWnd,
		(HMENU)ID_CANVAS,//id
		m_hInst);
	SetCanvasPos();
	Canvas.SetDoubleBuffer(true);
	Canvas.ShowWindow(TRUE);
	Canvas.UpdateWindow();

	_tcscpy(szFileName, TEXT(""));

	TCHAR szInputFileName[MAX_PATH];
	if (GetCommandLineByIndex(1, szInputFileName))//如果输入了文件
	{
		//ShowMessage(szInputFileName);
		_tcscpy(szFileName, szInputFileName);
		::PostMessage(hWnd, WM_COMMAND, MAKELONG(ID_OPEN, ID_OPEN_NOCHECK), 0);//无弹窗打开
	}
	else
		//默认初始化
		this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);//不检查状态

#ifdef _STUDENT
	PostMessage(hWnd, WM_COMMAND,MAKELONG(ID_SHOW_STUDENT_DIALOG,0), 0);
#endif

#ifdef _TEACHER
	//添加Menu
	HMENU hMenu = GetMenu(hWnd);//取得菜单句柄
	HMENU hMenuView = GetSubMenu(hMenu, 0);//“文件”菜单
	InsertMenu(hMenuView, 5, MF_SEPARATOR | MF_BYPOSITION, 0, TEXT(""));
	InsertMenu(hMenuView, 5, MF_ENABLED | MF_BYPOSITION, MAKELONG(ID_RECORD_SCORE, 0), TEXT("登记成绩"));

	//AppendMenu(hMenuData, MF_ENABLED, ID_MENU_GRAPH_DATA_START, PointData.sLegend.c_str());
#endif
}


void TMainWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	if (wmId == IDR_TRACKBAR)
	{
		//拖动了Trackbar
		m_Configuration.SetDPU(m_Trackbar.GetNowValue());

		m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："));

		TCHAR temp[64];
		TTransfer::double2TCHAR_AutoTrim0(m_Configuration.GetProportion() * 100, temp);
		m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%s%%"), temp);

		Canvas.Invalidate();
	}
}


void TMainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	static TCHAR lpstrFilterLML[] = TEXT("机构设计文件(*.lml)\0*.lml\0\0");

	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	TManageTool *p_Managetool = &m_ManageTool;
	switch (wmId)
	{
	case ID_NEW:
		if (wmEvent != ID_NEW_NOCHECK)//检查状态
			if (_tcslen(szFileName) > 0 || m_Shape.Element.size() > 0)
			{
				if (MessageBox(m_hWnd, TEXT("是否新建文件？"), TEXT(""), MB_YESNO) == IDYES)
				{
		//清空Student
#ifdef _STUDENT
		PostMessage(m_hWnd, WM_COMMAND, MAKELONG(ID_SHOW_STUDENT_DIALOG, 0), 0);
#endif

				}
				else
					break;
			}

		//不检查状态
		_tcscpy(szFileName, TEXT(""));
		SetText(szName);
		m_ManageTool.CloseCurTool();//关闭工具
		m_Shape.ReleaseAll();//释放元素
		RightWindow.TreeViewContent.DeleteAllItems();//删除tree
		RightWindow.ListView.DeleteAllItems();//删除list

		RightWindow.TreeViewContent.Initial();//树状图初始化
		this->m_Configuration.SetOrg(this->Canvas.ClientRect.right / 2, this->Canvas.ClientRect.bottom / 2);//设置原点
		m_Trackbar.SetToMid();
		OnCommand(MAKELONG(ID_SELECT, 0), 0);//设置选择工具

		pSolver->RefreshEquations();

		//清空Student
#if (defined _STUDENT) || (defined _TEACHER)
		sStudentClass.clear();
		sStudentName.clear();
		sStudentNumber.clear();
		sStudentScore.clear();
#endif

		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_OPEN:
		if (m_Shape.Element.size() > 0 && _tcslen(szFileName) > 0)//有数据且已有当前文件名
		{
			if (MessageBox(m_hWnd, TEXT("文件未保存，仍然要打开文件？"), NULL, MB_YESNO) == IDNO)
				return;
		}

		if (wmEvent != ID_OPEN_NOCHECK)
		{
			//准备打开对话框
			if (OpenFileDialog(m_hWnd, szFileName, lpstrFilterLML) == FALSE)
				break;
		}


		//因为New会改变szFileName，所以先保存再恢复
		TCHAR szFileNameBackup[MAX_PATH];
		_tcscpy(szFileNameBackup, szFileName);
		this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);//无条件初始化
		_tcscpy(szFileName, szFileNameBackup);

		if (m_Shape.ReadFromFile(szFileName))//读取文件
		{
			RightWindow.TreeViewContent.AddAllItem();
			SetText(TEXT("%s - %s"), szName, szFileName);
			pSolver->RefreshEquations();
		}
		else
		{//读取失败
			if (GetLastError() == ERROR_NO)//
				MessageBox(NULL, TEXT("文件格式不正确，读取中止。"), TEXT(""), MB_OK);
			else
				ShowMessage(TEXT("Error:%d"), GetLastError());

			this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);
		}

		break;
	case ID_SAVE:
	{
		if (wmEvent != ID_SAVE_NOCHECK)//无条件保存不弹对话框，略过此if
			if (_tcslen(szFileName) == 0 || GetFileExists(szFileName) == false)//没有当前文件或者当前文件已失效
			{
				//弹出对话框
				if (SaveFileDialog(m_hWnd, szFileName, lpstrFilterLML, TEXT("lml")) == FALSE)
					return;
			}

		if (!m_Shape.SaveToFile(szFileName))
			ShowMessage(TEXT("Error:%d"), GetLastError());
		else
		{
			ShowMessage(TEXT("保存%s成功。"), szFileName);
			SetText(TEXT("%s - %s"), szName, szFileName);
		}
	}
	break;
	case ID_SAVEAS:
	{
		if (SaveFileDialog(m_hWnd, szFileName, lpstrFilterLML, TEXT("lml")) == FALSE)
			return;

		this->OnCommand(MAKELONG(ID_SAVE, ID_SAVE_NOCHECK), 0);

		break;
	}
	case ID_EXIT:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case ID_SELECT:
	case ID_DRAG:
	case ID_DRAW_BAR:
	case ID_DRAW_FRAME:
	case ID_DRAW_LINE:
	case ID_DRAW_POLYLINE_BAR:
	case ID_DRAW_SLIDEWAY:
	case ID_DRAW_SLIDER:
	case ID_DRAW_COINCIDE:
	case ID_DRAW_COLINEAR:
		SendMessage(m_Toolbar.m_hWnd, WM_USER, wmId, 0);//刷新Toolbar里的radio状态
		p_Managetool->SetCurActiveTool(wmId);
		break;
	case ID_DELETE_ELEMENT:
		p_Managetool->m_pCurrentTool->OnKeyDown(m_hWnd, VK_DELETE, 0);
		break;
	case IDR_STATUS_CHECKBOX_SHOW_REAL:
		if (m_CheckBoxShowReal.GetChecked())
		{
			m_CheckBoxShowReal.SetBitmapIsChecked();
			m_Configuration.bDrawReal = true;
		}
		else
		{
			m_CheckBoxShowReal.SetBitmapIsUnChecked();
			m_Configuration.bDrawReal = false;
		}
		Canvas.Invalidate();
		break;
	case IDR_STATUS_CHECKBOX_THEME:
		if (m_CheckBoxTheme.GetChecked())
		{
			m_CheckBoxTheme.SetBitmapIsChecked();
			m_Configuration.bThemeDark = true;
			m_Configuration.SetTheme(true);
		}
		else
		{
			m_CheckBoxTheme.SetBitmapIsUnChecked();
			m_Configuration.bThemeDark = false;
			m_Configuration.SetTheme(false);
		}
		Canvas.Invalidate();
		break;
	case IDR_STATUS_CHECKBOX_AXES:
		if (m_CheckBoxAxes.GetChecked())
		{
			m_CheckBoxAxes.SetBitmapIsChecked();
			m_Configuration.bDrawAxes = true;
		}
		else
		{
			m_CheckBoxAxes.SetBitmapIsUnChecked();
			m_Configuration.bDrawAxes = false;
		}
		Canvas.Invalidate();
		break;
	case ID_ANALYZE_MECHANISM://分析机构
		if (pConsole == NULL)
			OnCommand(ID_SHOW_CONSOLE, 0);
		//pSolver->RefreshEquations(true);

		pSolver->ClearOutput();
		//pSolver->ClearEuqations();
		pSolver->RefreshEquations();

		break;
	case ID_SHOW_CONSOLE:
	{
		if (pConsole != NULL)
			pConsole->ShowWindow(SW_SHOWNORMAL);
		else
		{
			pConsole = new TConsole;
			pConsole->pSolver = pSolver;
			pConsole->CreateEx(0, TEXT("Console"), TEXT("Console"),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				m_hWnd, (HMENU)0, m_hInst);
			pConsole->ShowWindow(SW_SHOWNORMAL);
			pConsole->UpdateWindow();
			//pConsole->MessageLoop();
		}

		break;
	}
	case ID_DRAW_GRAPH:
	{
		TGraph *pGraph = new TGraph(&m_Configuration);
		pGraph->bShowMouseLine = true;
		pGraph->CreateEx(0, TEXT("图表"), TEXT("图表"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_hWnd, (HMENU)LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_MENU_GRAPH)), m_hInst);
		pGraph->SetDoubleBuffer(true);
		pGraph->ShowWindow(SW_SHOWNORMAL);


		pGraph->UpdateWindow();


		std::vector<DPOINT> dptVector, dptVector2;
		DPOINT dpt;
		srand(GetTickCount());
		double b = rand();
		double t = double(rand() % 10) / double(rand() % 100);
		for (int x = 0; x < 50 * (rand() % 19 + 1); ++x)
			//for (int x = -99; x <=100; ++x)
		{
			dpt.x = b + x*t;
			//dpt.x = x;
			dpt.y = sin(x / 10.0) * 50;
			dptVector.push_back(dpt);

			dpt.y = cos(x / 10.0);
			dptVector2.push_back(dpt);
		}

		pGraph->InputDptVector(dptVector, { PS_SOLID, { 1, 0 }, 0 }, true, TEXT("y=sin(x/50)"), TEXT("mm/s"));
		pGraph->InputDptVector(dptVector2, { PS_SOLID, { 1, 0 }, RGB(255, 0, 0) }, true, TEXT("y=cos(x/50)"), TEXT("mm^2/s"));

		//pGraph->SetMargin(40);

		pGraph->sLabelX = TEXT("x");
		//pGraph->sLabelY = TEXT("y");

		vecpGraph.push_back(pGraph);
		break;
	}
	case ID_SET_DRIVER:
	{
		if (m_ManageTool.m_uiCurActiveTool == ID_SELECT || m_ManageTool.m_uiCurActiveTool == ID_DRAG)
		{
			TSelectTool *pSelect = (TSelectTool *)m_ManageTool.m_pCurrentTool;
			TElement *pElement=nullptr;
			if (pSelect->CanBeDriver(pElement))//选择工具 且 选中元素可驱动 则弹出原动件对话框
			{
					//预设
					DialogAddDriver::iElementId = pElement->id;
					DialogAddDriver::dElementValue = pElement->angle;

				if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_DRIVER), m_hWnd, DialogAddDriver::DlgAddDriverProc))
				{
					MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
				}
			}
			else
				MessageBox(m_hWnd, TEXT("请先使用选择工具选择一个元素，再设为原动件。"), TEXT(""), MB_ICONINFORMATION);
		}
		else
			MessageBox(m_hWnd, TEXT("请先使用选择工具选择一个元素，再设为原动件。"), TEXT(""), MB_ICONINFORMATION);
		break;
	}
	case ID_ANIMATION:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ANIMATION), m_hWnd, DialogAnimation::DlgAnimationProc))
		{
			MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_OPTION:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_OPTION), m_hWnd, DlgOptionProc))
		{
			MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_ABOUT:
	{
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), m_hWnd, DlgAboutProc))
		{
			MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_DELETE_GRAPH:
		for (auto iter = vecpGraph.begin(); iter != vecpGraph.end(); ++iter)
		{
			if (*iter == (TGraph *)lParam)
			{
				delete *iter;
				vecpGraph.erase(iter);
				break;
			}
		}
		break;
	case ID_DELETE_CONSOLE:
		if (pConsole != NULL)
		{
			delete pConsole;
			pConsole = NULL;
		}
		break;
	case ID_REFRESH:
		m_Shape.SimplifyPhiValue();
		pSolver->RefreshEquations();
		pSolver->ClearOutput();
		pSolver->ClearConstraint();
		//pSolver->AddMouseConstraint(false, iPickIndex, pConfig->ScreenToReal(ptPos));
		pSolver->Solve();
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_SAVE_CAPTURE:
	{
		TCHAR szFileNamePNG[MAX_PATH];
		if (SaveFileDialog(m_hWnd, szFileNamePNG, TEXT("PNG File\0*.png\0\0"), TEXT("png")))
		{
			if (TDraw::CaptureWindowToFile(Canvas.m_hWnd, szFileNamePNG))
				MessageBox(NULL, TEXT("保存成功。"), TEXT(""), 0);
			else
				MessageBox(NULL, TEXT("保存失败。"), TEXT(""), MB_ICONERROR);
		}
		break;
	}
	case ID_VIEW_SUITABLE:
	{
		RECT rcObj;
		POINT ptObjCenter;
		if (m_Shape.GetBoundingBox(rcObj, ptObjCenter, &m_Configuration) == false)
			break;

		RECT rcWin = Canvas.ClientRect;
		POINT ptWinCenter;
		TDraw::GetCenter(ptWinCenter, rcWin);

		double ProOrg = m_Configuration.GetProportion();

		double lambda_x = double(rcWin.right) / (rcObj.right - rcObj.left);
		double lambda_y = double(rcWin.bottom) / (rcObj.bottom - rcObj.top);

		double lambda = min(lambda_x, lambda_y)*ProOrg;

		//int StepCount = 20;
		//int Time = 2000; //ms
		//for (int i = 1; i <= StepCount; ++i)
		//{
		//	m_Configuration.SetOrg(ptOrg.x + double(dx)*i / StepCount, ptOrg.y + double(dy)*i / StepCount);
		//	Canvas.Invalidate();
		//	Sleep(Time/StepCount);
		//}

		if (lambda < ProOrg)//缩小
		{
			do
			{
				if (m_Trackbar.GetPos() > 0)
					m_Trackbar.SetPos(m_Trackbar.GetPos() - 1);
			} while (lambda < m_Trackbar.GetNowValue() );//
		}
		else
		{
			while (lambda > m_Trackbar.GetNowValue() && (lambda-m_Trackbar.GetNowValue())/m_Trackbar.GetNowValue()>1.0)
			{
				if (m_Trackbar.GetPos() < m_Trackbar.GetRange() - 1)
					m_Trackbar.SetPos(m_Trackbar.GetPos() + 1);
			}
		}

		m_Shape.GetBoundingBox(rcObj, ptObjCenter, &m_Configuration);
		POINT ptOrg = m_Configuration.GetOrg();
		LONG dx = ptWinCenter.x - ptObjCenter.x;
		LONG dy = ptWinCenter.y - ptObjCenter.y;
		m_Configuration.SetOrg(ptOrg.x + dx, ptOrg.y + dy);

		Canvas.Invalidate();
	}
	break;
	case ID_SHOW_STUDENT_DIALOG:
#ifdef _STUDENT
		//打开学生窗口
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_STU), m_hWnd, DialogStudent::DlgStudentProc))
		{
			MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
		}
#endif
		break;
	case ID_RECORD_SCORE:
#ifdef _TEACHER
		//打开成绩登记窗口
		if (-1 == DialogBox(m_hInst, MAKEINTRESOURCE(IDD_DIALOG_SCORE), m_hWnd, DialogScore::DlgScoreProc))
		{
			MessageBox(NULL, TEXT("窗口打开失败。"), TEXT(""), MB_ICONERROR);
		}
		if (GetFileExists(szFileName))
			this->OnCommand(MAKELONG(ID_SAVE, ID_SAVE_NOCHECK), 0);
#endif
		break;
	case IDR_LINEEDIT://not deal
		break;
	default:
		assert(0);
		break;
	}
}

void TMainWindow::SetCanvasPos()
{
	::MoveWindow(Canvas.m_hWnd,
		ClientRect.left,
		ClientRect.top + m_Toolbar.GetClientRect().bottom,
		ClientRect.right - ClientRect.left - (RightWindow.ClientRect.right - RightWindow.ClientRect.left),
		ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom, true);
}

void TMainWindow::SetRightWindowPos()
{
	::MoveWindow(RightWindow.m_hWnd,
		ClientRect.right - m_iRightWindowWidth,
		ClientRect.top + m_Toolbar.GetClientRect().bottom,
		m_iRightWindowWidth,
		ClientRect.bottom - m_Toolbar.GetClientRect().bottom - m_Status.GetClientRect().bottom, true);
}

//只有此处会变更布局大小
void TMainWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SIZE_MINIMIZED)
		return;
	m_Toolbar.FreshSize();
	m_Status.FreshSize();
	m_Trackbar.SetRect(m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0));//Trackbar嵌入Status
	m_CheckBoxShowReal.SetPositionOnlyOrigin(m_Status.GetPartRect(IDR_STATUS_CHECKBOX_SHOW_REAL, 1));
	m_CheckBoxTheme.SetPositionOnlyOrigin(m_Status.GetPartRect(IDR_STATUS_CHECKBOX_THEME, 1));
	m_CheckBoxAxes.SetPositionOnlyOrigin(m_Status.GetPartRect(IDR_STATUS_CHECKBOX_AXES, 1));

	SetRightWindowPos();


	//记录下坐标原点相对比例，Canvas变动后再按比例恢复坐标位置
	DPOINT OrgProportion;
	if (Canvas.ClientRect.right != 0 && Canvas.ClientRect.bottom != 0)
	{
		OrgProportion.x = (double)m_Configuration.GetOrg().x / (double)Canvas.ClientRect.right;
		OrgProportion.y = (double)m_Configuration.GetOrg().y / (double)Canvas.ClientRect.bottom;
	}
	else
		OrgProportion = { 1.0, 1.0 };

	SetCanvasPos();

	m_Configuration.SetOrg((LONG)(OrgProportion.x*Canvas.ClientRect.right), (LONG)(OrgProportion.y*Canvas.ClientRect.bottom));

	Canvas.Invalidate();

}
