#pragma once
#include <vector>
#include <Windows.h>
#include "String.h"

class TElement;
namespace DialogAnimation
{
	enum enumListBoxItemType{ P, V, A };
	enum enumListBoxItemValueType{ X, Y, PHI };
	struct TListBoxItem
	{
		int id;
		int index_of_point;
		enumListBoxItemType type;//P,V,A
		enumListBoxItemValueType value_type;//X,Y,PHI
		String s;
		std::vector<double> data;
		TElement *pElement;
	};

	BOOL CALLBACK DlgAnimationProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void SetEditTimeAndEditFrame();
	void SetMesureControlEnable(bool bEnable);
	void SetPlayerEnable(bool bEnable);
	void SetPlayerButtonEnableWhenDragTrackbar();
	VOID AnalyzeProc(PVOID pvoid);
	VOID PlayProc(PVOID pvoid);
	void InitialListBoxLeft();
}