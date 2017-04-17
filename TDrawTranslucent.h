#pragma once
#include <Windows.h>

class TDrawTranslucent
{
private:
	HDC *phdc;
	HDC hdcOld;
	HDC hBitmapDC;
	HBITMAP hBitmap;
	VOID *pvBits;
	bool bNeedDrawBlack;
	BYTE alpha;
	long left, top, width, height;
	static void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack);
	static void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, bool bNeedDrawBlack);
	static void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack);
	static void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, BYTE alpha, bool bNeedDrawBlack);
public:
	TDrawTranslucent();
	~TDrawTranslucent();

	//执行Start后，hdc将变为hBitmap，之后直接使用hdc绘图即可。在End中将恢复原hdc
	void TDrawTranslucent::Start(HDC &hdc, double alpha, long left, long top, long width, long height, bool bNeedDrawBlack);
	void TDrawTranslucent::Start(HDC &hdc, double alpha, const RECT &rect, bool bNeedDrawBlack);
	void TDrawTranslucent::End();
};

