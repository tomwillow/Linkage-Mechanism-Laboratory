#pragma once
#include "MyMath.h"
#include <initializer_list>
#include <vector>

#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

#include "TControl.h"

class TTrackbar:public TControl
{
private:
	std::vector<double> vecValue;
public:
	TTrackbar();
	~TTrackbar();
	void TTrackbar::CreateTrackbar(HWND hParent, HINSTANCE hInst, RECT rc, UINT id);
	void TTrackbar::SetPos(int pos);
	int TTrackbar::GetPos();
	void TTrackbar::SetRange(int num);
	int TTrackbar::GetRange();

	//只支持升序排列
	void TTrackbar::SetRangeAndValue(std::initializer_list<double> init_list)
	{
		for (auto value : init_list)
			vecValue.push_back(value);
		SetRange(vecValue.size()-1);
	}

	void TTrackbar::SetToMid()
	{
		SetPos(GetRange() / 2);
	}

	double TTrackbar::GetValue(int pos)
	{
		if (pos < vecValue.size())
			return vecValue[pos];
		else
			return 0.0;
	}

	double TTrackbar::GetNowValue()
	{
		return GetValue(GetPos());
	}

	double TTrackbar::GetPrevValue()
	{
		if (GetPos()-1 >= 0)
			return GetValue(GetPos() - 1);
		else
			return GetNowValue();
	}

	double TTrackbar::GetNextValue()
	{
		if (GetPos() + 1 < GetRange()+1)
			return GetValue(GetPos() + 1);
		else
			return GetNowValue();
	}

	void TTrackbar::SetPosByValue(double value, double precision)
	{
		for (size_t i = 0; i < vecValue.size(); ++i)
			if (ISEQUAL(vecValue[i], value, precision))
			{
				SetPos(i);
				break;
			}
	}

	double TTrackbar::GetMaxValue()
	{
		return vecValue.back();
	}

	double TTrackbar::GetMinValue()
	{
		return vecValue.front();
	}
};

