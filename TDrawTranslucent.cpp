#include "TDrawTranslucent.h"
#include "DetectMemoryLeak.h"
#include "TDraw.h"

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
	//所有开始经过此处
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

void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack)
{
	if (pvBits)
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
	}
#ifdef _DEBUG
	else
		OutputDebugPrintf(TEXT("pvBits == false\n"));
#endif
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
void TDrawTranslucent::Start(HDC &hdc, byte alpha, long input_left, long input_top, long input_width, long input_height, bool bNeedDrawBlack)
{
	//OutputDebugPrintf(TEXT("il%d it%d iw%d ih%d\n"), input_left, input_top, input_width, input_height);
	//所有开始经过此处
	if (input_width < 0)
	{
		this->left = input_left + input_width;
		this->width = -input_width;
		bExchangedX = true;
	}
	else
	{
		this->left = input_left;
		this->width = input_width;
		bExchangedX = false;
	}

	if (input_height < 0)
	{
		this->top = input_top + input_height;
		this->height = -input_height;
		bExchangedY = true;
	}
	else
	{
		this->top = input_top;
		this->height = input_height;
		bExchangedY = false;
	}
	//OutputDebugPrintf(TEXT("x%d y%d w%d h%d\n"), this->left, this->top, this->width, this->height);

	this->phdc = &hdc;
	this->hdcOld = hdc;
	this->bNeedDrawBlack = bNeedDrawBlack;
	this->alpha = alpha;
	this->StartTranslucent(hBitmapDC, hBitmap, pvBits, left, top, width, height , bNeedDrawBlack);
	hdc = hBitmapDC;
}

//所有绘制x坐标均应-left，y坐标应-top
//只要画的不是黑色 背景就是黑色 -> bNeedDrawBlack=false
void TDrawTranslucent::Start(HDC &hdc, byte alpha, const RECT &rect, bool bNeedDrawBlack)
{
	Start(hdc, alpha, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bNeedDrawBlack);
}

void TDrawTranslucent::End()
{
	this->EndTranslucent(hdcOld, hBitmapDC, hBitmap, pvBits, left,top,width,height, alpha, bNeedDrawBlack);
	
	//恢复处理
	*phdc = hdcOld;
	for (auto &pRect:vecpRect)
		Restore(*pRect);

	for (auto &tapt : vecapt)
		Restore(tapt.apt, tapt.count);
}

void TDrawTranslucent::Deal(RECT &rect)
{
	TDraw::MoveRect(rect, left, top);
	if (bExchangedX)
		std::swap(rect.left, rect.right);
	if (bExchangedY)
		std::swap(rect.top, rect.bottom);
}
void TDrawTranslucent::Restore(RECT &rect)
{
	TDraw::MoveRect(rect, -left, -top);
	if (bExchangedX)
		std::swap(rect.left, rect.right);
	if (bExchangedY)
		std::swap(rect.top, rect.bottom);
}

void TDrawTranslucent::Deal(POINT *apt, int count)
{
	TDraw::MoveByDelta(apt, count, -left, -top);
}

void TDrawTranslucent::Restore(POINT *apt, int count)
{
	TDraw::MoveByDelta(apt, count, left, top);
}

TDrawTranslucent& TDrawTranslucent::Input(RECT *rect)
{
	Deal(*rect);
	vecpRect.push_back(rect);
	return *this;
}

TDrawTranslucent& TDrawTranslucent::Input(POINT *point)
{
	return Input(point, 1);;
}
TDrawTranslucent& TDrawTranslucent::Input(POINT *point, int count)
{
	Deal(point, count);
	vecapt.push_back({ point, count });
	return *this;
}