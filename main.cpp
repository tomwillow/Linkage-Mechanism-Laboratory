#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#include "ControlStyle.h"
#include "resource.h"

#include "KWindow.h"
#include "TMainWindow.h"
#include "TCanvas.h"
#include "TManageTool.h"
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"comctl32.lib")//样式使用

TMainWindow win;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_WIN95_CLASSES; // Or ICC_PROGRESS_CLASS
	if (!InitCommonControlsEx(&iccx))
		return FALSE;

	const TCHAR szAppTitle[] = TEXT("机构设计系统");

	win.CreateEx(0, szAppTitle,szAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, (HMENU)LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENUMAIN)), hInstance);
	//win.SetDoubleBuffer(true);
	win.SetAccel(LoadAccelerators(hInstance, (LPCTSTR)IDR_ACCELERATOR_MAIN));
	win.ShowWindow(iCmdShow);

	win.UpdateWindow();

	return win.MessageLoop();
}