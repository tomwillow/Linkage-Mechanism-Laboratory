#pragma once
#include "TMainWindow.h"

#include "TConstraintCoincide.h" 
#include "TSolve.h"

TMainWindow::TMainWindow()
{
	m_iRightWindowWidth = 200;
}


TMainWindow::~TMainWindow()
{
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
	m_Toolbar.AddButton(0, ID_SELECT, true, TEXT("选择"));
	m_Toolbar.AddSeparator(0);
	m_Toolbar.AddButton(1, ID_DRAW_FRAME, true, TEXT("机架"));
	m_Toolbar.AddButton(2, ID_DRAW_BAR, true, TEXT("连杆"));
	m_Toolbar.AddButton(3, ID_DRAW_LINE, true, TEXT("线"));
	m_Toolbar.AddButton(4, ID_ANALYZE_MECHANISM, true, TEXT("分析机构"));
	m_Toolbar.ShowToolbar();

	//创建状态栏
	m_Status.Create(hWnd, IDR_STATUS, m_hInst, 24);
	m_Status.AddPart(IDR_STATUS_COORDINATE, 160, PT_FIXED);
	m_Status.AddPart(0, 0, PT_NONE);
	m_Status.AddPart(IDR_STATUS_PROPORTIONNAME, 40, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_TRACKBAR, 120, PT_FIXED);
	m_Status.AddPart(IDR_STATUS_PROPORTION, 60, PT_FIXED);
	m_Status.AddPart(0, 6, PT_FIXED);
	m_Status.FreshSize();

	m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："), m_Configuration.GetProportion() * 100);
	m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);

	//创建Trackbar
	m_Trackbar.CreateTrackbar(m_Status.m_hWnd, this->m_hInst, m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0), IDR_TRACKBAR);
	m_Trackbar.SetRange(6);
	m_Trackbar.SetPos(3);

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

	//默认初始化
	this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);

}


void TMainWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	if (wmId == IDR_TRACKBAR)
	{
		//拖动了Trackbar
		switch (m_Trackbar.GetPos())
		{
		case 0:
			m_Configuration.SetDPU(0.125); break;
		case 1:
			m_Configuration.SetDPU(0.25); break;
		case 2:
			m_Configuration.SetDPU(0.5); break;
		case 3:
			m_Configuration.SetDPU(1.0); break;
		case 4:
			m_Configuration.SetDPU(2.0); break;
		case 5:
			m_Configuration.SetDPU(4.0); break;
		case 6:
			m_Configuration.SetDPU(8.0); break;
		}
		m_Status.SetText(IDR_STATUS_PROPORTIONNAME, TEXT("比例："), m_Configuration.GetProportion() * 100);
		m_Status.SetText(IDR_STATUS_PROPORTION, TEXT("%.0f%%"), m_Configuration.GetProportion() * 100);
		InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, false);
	}
}

bool GetFileExists(TCHAR filename[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(filename, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

void InitialOpenFileName(OPENFILENAME *ofn, HWND hwnd, TCHAR szFile[], DWORD nMaxFile = MAX_PATH)
{
	// Initialize OPENFILENAME
	ZeroMemory(ofn, sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = hwnd;
	ofn->lpstrFile = szFile;
	ofn->lpstrFile[0] = TEXT('\0');
	ofn->nMaxFile = nMaxFile;
	ofn->lpstrFilter = TEXT("机构设计文件(*.mds)\0*.mds\0\0");
	ofn->nFilterIndex = 1;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	ofn->lpstrInitialDir = NULL;
}

void TMainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	TManageTool *p_Managetool = &m_ManageTool;
	switch (wmId)
	{
	case ID_NEW:
		if (wmEvent != ID_NEW_NOCHECK)
			if (_tcslen(szFileName) > 0 || m_Shape.Element.size()>0)
			{
				if (MessageBox(m_hWnd, TEXT("是否新建文件？"), TEXT(""), MB_YESNO) == IDNO)
					break;
			}
		_tcscpy(szFileName, TEXT(""));
		SetText(szName);
		m_ManageTool.CloseCurTool();
		m_Shape.ReleaseAll();
		RightWindow.TreeViewContent.DeleteAllItems();
		RightWindow.ListView.DeleteAllItems();

		RightWindow.TreeViewContent.Initial();
		this->m_Configuration.SetOrg(this->Canvas.ClientRect.right / 2, this->Canvas.ClientRect.bottom / 2);
		m_ManageTool.SetCurActiveTool(ID_SELECT);
		::InvalidateRect(Canvas.m_hWnd, &(Canvas.ClientRect), FALSE);
		break;
	case ID_OPEN:
		if (m_Shape.Element.size() > 0 && _tcslen(szFileName) > 0)//有数据且已有当前文件名
		{
			if (MessageBox(m_hWnd, TEXT("文件未保存，仍然要打开文件？"), NULL, MB_YESNO) == IDNO)
				return;
		}
		this->OnCommand(MAKELONG(ID_NEW, ID_NEW_NOCHECK), 0);//无条件初始化

		OPENFILENAME ofn;
		HANDLE hf;

		InitialOpenFileName(&ofn, m_hWnd, szFileName);
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == TRUE)
		{
			hf = CreateFile(ofn.lpstrFile,
				GENERIC_READ,
				0,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL);
			if (GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}

			DWORD *now_pos = new DWORD;
			int size;
			ReadFile(hf, &size, sizeof(int), now_pos, NULL);
			if (GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}
			*now_pos += sizeof(int);

			EnumElementType eType;
			for (int i = 0; i < size; i++)
			{
				ReadFile(hf, &eType, sizeof(EnumElementType), now_pos, NULL);
				if (GetLastError() != 0)
				{
					ShowMessage(TEXT("Error:%d"), GetLastError());
					return;
				}
				*now_pos += sizeof(EnumElementType);

				switch (eType)
				{
				case ELEMENT_BAR:
				{
					TBar *temp = new TBar;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
						return;
					}
					m_Shape.AddBar(temp);
					delete temp;
					break;
				}
				case ELEMENT_REALLINE:
				{
					TRealLine *temp = new TRealLine;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
						return;
					}
					m_Shape.AddRealLine(*temp);
					delete temp;
					break;
				}
				case ELEMENT_FRAMEPOINT:
				{
					TFramePoint *temp = new TFramePoint;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
						return;
					}
					m_Shape.AddFramePoint(*temp);
					delete temp;
					break;
				}
				case CONSTRAINT_COINCIDE:
				{
					TConstraintCoincide *temp = new TConstraintCoincide;
					ReadFile(hf, temp, m_Shape.GetSizeOfElement(eType), now_pos, NULL);
					if (GetLastError() != 0)
					{
						ShowMessage(TEXT("Error:%d"), GetLastError());
						return;
					}
					m_Shape.AddCoincide(*temp);
					delete temp;
					break;
				}
				}
			}

			CloseHandle(hf);
			delete now_pos;
			RightWindow.TreeViewContent.AddAllItem();
			SetText(TEXT("%s - %s"), szName, szFileName);
		}
		break;
	case ID_SAVE:
	{
		if (wmEvent != ID_SAVE_NOCHECK)
			if (_tcslen(szFileName) == 0 || GetFileExists(szFileName) == false)//没有当前文件或者当前文件已失效
			{
				OPENFILENAME ofn;
				InitialOpenFileName(&ofn, m_hWnd, szFileName);

				ofn.Flags = OFN_PATHMUSTEXIST;
				ofn.lpstrDefExt = TEXT("mds");

				if (GetSaveFileName(&ofn) == FALSE)
					return;
			}

		//无弹窗直接存储
		HANDLE hf;
		hf = CreateFile(szFileName,
			GENERIC_WRITE,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);
		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		{
			ShowMessage(TEXT("Error:%d"), GetLastError());
			return;
		}
		DWORD *now_pos = new DWORD;
		*now_pos = 0;

		//写入元素数量
		int size = m_Shape.Element.size();
		WriteFile(hf, &size, sizeof(int), now_pos, NULL);
		*now_pos += sizeof(int);
		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		{
			ShowMessage(TEXT("Error:%d"), GetLastError());
			return;
		}

		for (int i = 0; i < m_Shape.Element.size(); i++)
		{
			//写入类型标记
			WriteFile(hf, &(m_Shape.Element[i]->eType), sizeof(EnumElementType), now_pos, NULL);
			*now_pos += sizeof(EnumElementType);
			if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}

			//写入数据
			WriteFile(hf, m_Shape.Element[i], m_Shape.GetSizeOfElement(m_Shape.Element[i]->eType), now_pos, NULL);
			*now_pos += m_Shape.GetSizeOfElement(m_Shape.Element[i]->eType);
			if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
			{
				ShowMessage(TEXT("Error:%d"), GetLastError());
				return;
			}
		}
		delete now_pos;
		CloseHandle(hf);
		ShowMessage(TEXT("保存%s成功。"), szFileName);
		SetText(TEXT("%s - %s"), szName, szFileName);

	}
	break;
	case ID_SAVEAS:
	{
		OPENFILENAME ofn;
		InitialOpenFileName(&ofn, m_hWnd, szFileName);

		ofn.Flags = OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = TEXT("mds");

		if (GetSaveFileName(&ofn) == FALSE)
			return;
		this->OnCommand(MAKELONG(ID_SAVE, ID_SAVE_NOCHECK), 0);
		break;
	}
	case ID_EXIT:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case ID_SELECT:
	case ID_DRAW_BAR:
	case ID_DRAW_FRAME:
	case ID_DRAW_LINE:
		p_Managetool->SetCurActiveTool(wmId);
		break;
	case ID_ANALYZE_MECHANISM:
	{
		//TWindow w;	
		//w.CreateEx(0, TEXT("j"), TEXT("j"),
		//	WS_OVERLAPPEDWINDOW,
		//	CW_USEDEFAULT,
		//	CW_USEDEFAULT,
		//	CW_USEDEFAULT,
		//	CW_USEDEFAULT,
		//	m_hWnd, (HMENU)0, m_hInst);
		//w.ShowWindow(SW_SHOW);
		//w.UpdateWindow();
		//w.MessageLoop();
		TSolve Solve;
		Solve.Solute();

		break;
	}
	case ID_VIEW_SUITABLE:
		DPOINT center;
		double left, right, top, bottom;
		for (int i = 0; i < m_Shape.Element.size(); i++)
		{
			switch (m_Shape.Element[i]->eType)
			{
			case ELEMENT_BAR:
			case ELEMENT_REALLINE:

				break;
			}
		}
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
	m_Trackbar.MoveWindow(m_Status.GetPartRect(IDR_STATUS_TRACKBAR, 0));//Trackbar嵌入Status

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

	::InvalidateRect(Canvas.m_hWnd, &Canvas.ClientRect, FALSE);
}
