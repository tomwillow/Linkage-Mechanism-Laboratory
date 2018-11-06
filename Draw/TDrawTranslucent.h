#pragma once
#include <vector>
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

	bool bExchangedX, bExchangedY;//由于出现负宽度及负高度导致的X,Y坐标交换

	std::vector<RECT *> vecpRect;
	struct TAPT
	{
		POINT *apt;
		int count;
	};
	std::vector<TAPT> vecapt;
	static void TDrawTranslucent::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack);
	static void TDrawTranslucent::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack);

	void TDrawTranslucent::Deal(RECT &rect);
	void TDrawTranslucent::Restore(RECT &rect);
	void TDrawTranslucent::Deal(POINT *apt,int count);
	void TDrawTranslucent::Restore(POINT *apt,int count);

public:
	TDrawTranslucent();
	~TDrawTranslucent();

	//执行Start后，hdc将变为hBitmap，之后直接使用hdc绘图即可。在End中将恢复原hdc
	void TDrawTranslucent::Start(HDC &hdc, byte alpha, long left, long top, long width, long height, bool bNeedDrawBlack);
	void TDrawTranslucent::Start(HDC &hdc, byte alpha, const RECT &rect, bool bNeedDrawBlack);
	void TDrawTranslucent::End();

	TDrawTranslucent& TDrawTranslucent::Input(RECT *rect);
	TDrawTranslucent& TDrawTranslucent::Input(POINT *point);
	TDrawTranslucent& TDrawTranslucent::Input(POINT *point, int count);
};

