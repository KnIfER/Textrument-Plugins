#include "StdAfx.h"
#include "DayButton.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(DayButtonUI)

	DayButtonUI::DayButtonUI()
		: CButtonUI()
	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	LPCTSTR DayButtonUI::GetClass() const
	{
		return _T("DayButton");
	}

	LPVOID DayButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_BUTTON) == 0 ) return static_cast<DayButtonUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT DayButtonUI::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}


	void DayButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CButtonUI::SetAttribute(pstrName, pstrValue);
	}





	//************************************
	// 函数名称: SetCalendarValDest
	// 返回类型: void
	// 参数信息: LPCTSTR pstrValue
	// 函数说明: 
	//************************************
	void DayButtonUI::SetCalendarValDest( LPCTSTR pstrValue )
	{
		m_sSalendarValDest = pstrValue;
	}

	//************************************
	// 函数名称: GetCalendarValDest
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR DayButtonUI::GetCalendarValDest()
	{
		return m_sSalendarValDest.GetData();
	}
}