#include "StdAfx.h"
#include "TestButton.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(TestButton)

	TestButton::TestButton() : Button()
	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	LPCTSTR TestButton::GetClass() const
	{
		return _T("DayButton");
	}

	LPVOID TestButton::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_BUTTON) == 0 ) return static_cast<TestButton*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT TestButton::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}


}