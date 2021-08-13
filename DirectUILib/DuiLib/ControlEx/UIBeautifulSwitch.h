#pragma once
#include "../StdAfx.h"

namespace DuiLib
{
	class UILIB_API CBSwitchUI : public COptionUI
	{
		DECLARE_DUICONTROL(CBSwitchUI)
	public:
		void DoEvent(TEventUI& event) override;

		LPCTSTR GetClass() const override;

		void PaintBkColor(HDC hDC) override;

		void PaintForeColor(HDC hDC) override;

		void PaintForeImage(HDC hDC) override;
	};
}