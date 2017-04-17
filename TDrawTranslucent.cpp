#include "TDrawTranslucent.h"


TDrawTranslucent::TDrawTranslucent()
{
}


TDrawTranslucent::~TDrawTranslucent()
{
}

//所有绘制x坐标均-left，y坐标-top
//只要画的不是黑色 背景就是黑色 -> bNeedDrawBlack=false
//标准开头（注销在EndTranslucent中完成，不需手动处理）
//HDC hBitmapDC;
//HBITMAP hBitmap;
//VOID *pvBits;
void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack)
{
	hBitmapDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;//正数，说明数据从下到上，如未负数，则从上到下  
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	hBitmap = CreateDIBSection(hBitmapDC, &bmpInfo, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	SelectObject(hBitmapDC, hBitmap);

	//开始画
	if (bNeedDrawBlack)//背景不是黑色就先涂白
	{
		HPEN hPen = (HPEN)::GetStockObject(NULL_PEN);
		::SelectObject(hBitmapDC, hPen);

		HBRUSH hBrush = CreateSolidBrush(0xffffff);//white

		RECT rcTrans = { 0, 0, width, height };
		::FillRect(hBitmapDC, &rcTrans, hBrush);

		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
}

//所有绘制x坐标均-left，y坐标-top
//只要画的不是黑色 背景就是黑色 -> bNeedDrawBlack=false
//标准开头（注销在EndTranslucent中完成，不需手动处理）
//HDC hBitmapDC;
//HBITMAP hBitmap;
//VOID *pvBits;
void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, bool bNeedDrawBlack)
{
	StartTranslucent(hBitmapDC, hBitmap, pvBits, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bNeedDrawBlack);
}

void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, BYTE alpha, bool bNeedDrawBlack)
{
	EndTranslucent(hdc, hBitmapDC, hBitmap, pvBits, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, alpha, bNeedDrawBlack);
}

void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack)
{

	//将有内容区域设为不透明
	UINT32 *data;
	if (bNeedDrawBlack)
		for (int i = 0; i < width*height; ++i)
		{
			data = ((UINT32 *)pvBits) + i;
			if (*data != 0x00ffffff)//not white
				*data |= 0xff000000;
		}
	else
		for (int i = 0; i < width*height; ++i)//黑色背景
		{
			data = ((UINT32 *)pvBits) + i;
			if (*data)//为黑色
				*data |= 0xff000000;
		}

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = alpha;

	AlphaBlend(hdc, left, top, width, height, hBitmapDC, 0, 0, width, height, bf);

	DeleteObject(hBitmap);
	DeleteObject(hBitmapDC);
}

//所有绘制x坐标均-left，y坐标-top
//只要画的不是黑色 背景就是黑色 -> bNeedDrawBlack=false
void TDrawTranslucent::Start(HDC &hdc, double alpha, long left, long top, long width, long height, bool bNeedDrawBlack)
{
	this->left = left;
	this->top = top;
	this->width = width;
	this->height = height;
	this->phdc = &hdc;
	this->hdcOld = hdc;
	this->bNeedDrawBlack = bNeedDrawBlack;
	this->alpha = alpha;
	this->StartTranslucent(hBitmapDC, hBitmap, pvBits, left, top, width, height , bNeedDrawBlack);
	hdc = hBitmapDC;
}

//所有绘制x坐标均-left，y坐标-top
//只要画的不是黑色 背景就是黑色 -> bNeedDrawBlack=false
void TDrawTranslucent::Start(HDC &hdc, double alpha, const RECT &rect, bool bNeedDrawBlack)
{
	Start(hdc, alpha, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bNeedDrawBlack);
}

void TDrawTranslucent::End()
{
	this->EndTranslucent(hdcOld, hBitmapDC, hBitmap, pvBits, left,top,width,height, alpha, bNeedDrawBlack);
	
	*phdc = hdcOld;
}