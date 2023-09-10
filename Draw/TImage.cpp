#include "..\Draw\TImage.h"


TImage::TImage(HWND hWnd)
{
	this->hWnd = hWnd;

	RECT rc;
	GetClientRect(hWnd, &rc);

	width = rc.right;
	height = rc.bottom;

	hdc = GetDC(hWnd);

	hBitmapDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -height;//正数，说明数据从下到上，如为负数，则从上到下  
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	hBitmap = CreateDIBSection(hBitmapDC, &bmpInfo, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	SelectObject(hBitmapDC, hBitmap);

	BitBlt(hBitmapDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
}


TImage::~TImage()
{
	DeleteObject(hBitmap);
	DeleteObject(hBitmapDC);

	ReleaseDC(hWnd, hdc);
}
