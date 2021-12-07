#define STRICT
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include "Common\String.h"
#include "Common\tchar_head.h"
#include <CommCtrl.h>

#include "Common\ControlStyle.h"
#include "resource.h"

#include "global.h"
#include "Control\TMainWindow.h"

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"comctl32.lib")//样式使用
#pragma comment(lib,"Comdlg32.lib")

TMainWindow win;
TMainWindow *pwin = &win;

std::String sStudentClass;
std::String sStudentName;
std::String sStudentNumber;
std::String sStudentScore;

#ifdef _STUDENT
	const TCHAR szAppTitle[] = TEXT("Linkage Mechanism Laboratory 学生版 v0.4");
#else
	#ifdef _TEACHER
	const TCHAR szAppTitle[] = TEXT("Linkage Mechanism Laboratory 教师版 v0.4");
	#else
	const TCHAR szAppTitle[] = TEXT("Linkage Mechanism Laboratory v0.4");
	#endif
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_WIN95_CLASSES; // Or ICC_PROGRESS_CLASS
	if (!InitCommonControlsEx(&iccx))
		return FALSE;


	win.LoadTitleIcon(hInstance, IDI_ICON);
	win.CreateEx(0, szAppTitle,szAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, (HMENU)LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU_MAIN)), hInstance);
	win.SetAccel(IDR_ACCELERATOR_MAIN);
	win.ShowWindow(iCmdShow);

	win.UpdateWindow();

	return win.MessageLoop();
}