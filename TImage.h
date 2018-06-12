#pragma once
#include <Windows.h>

class TImage
{
private:
	HWND hWnd; 
	HDC hdc;
	HDC hBitmapDC;
	HBITMAP hBitmap;
public:
	LONG width, height;

	/*
	按以下格式可遍历数据：
	UINT32 data;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			data = *((UINT32*)(pvBits)+y*width + x);//(height - 1 - y)
			*p++ = data >> 16;
			*p++ = data >> 8;
			*p++ = data;
		}		*/
	VOID* pvBits;
	TImage(HWND hWnd);
	~TImage();
};

