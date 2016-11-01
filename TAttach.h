#pragma once
#include <Windows.h>
#include "DPOINT.h"
#include "TShape.h"
#include "TConfiguration.h"
#include "TDraw.h"

class TAttach
{
private:
	TShape *Shape;
	TConfiguration *Config;
public:
	DPOINT dptAttach;
	bool bAttachLine;
	HWND hCanvas;
	TAttach();
	~TAttach();
	void TAttach::Load(HWND hCanvas, TShape *shape, TConfiguration *config);
	void TAttach::AttachAll(POINT ptPos);
	void TAttach::AttachLine(DPOINT ptDPos);
	void TAttach::Draw(HDC hdc);
};

