#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
		DECLARE_QKCONTROL(CLabelUI)
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		void NeedRecalcAutoSize();
		SIZE EstimateSize(const SIZE & szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
		virtual void SetText(LPCTSTR pstrText);
		//virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
		
	protected:
		DWORD	m_dwTextColor;
		DWORD	m_dwDisabledTextColor;
		UINT	m_uTextStyle;
		//RECT	m_rcTextPadding;
		bool	m_bShowHtml;

		int _preSizeX = 0;
	};
}

#endif // __UILABEL_H__