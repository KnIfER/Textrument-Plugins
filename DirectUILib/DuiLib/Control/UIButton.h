#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

struct _BUTTON_INFO;

namespace DuiLib
{
	class UILIB_API CButtonUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CButtonUI)

	public:
		CButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void Init() override;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		int Toggle();
		int GetCheckedValue();

		virtual LPCTSTR GetNormalImage();
		virtual void SetNormalImage(LPCTSTR pStrImage);
		virtual LPCTSTR GetHotImage();
		virtual void SetHotImage(LPCTSTR pStrImage);
		virtual LPCTSTR GetPushedImage();
		virtual void SetPushedImage(LPCTSTR pStrImage);
		virtual LPCTSTR GetFocusedImage();
		virtual void SetFocusedImage(LPCTSTR pStrImage);
		virtual LPCTSTR GetDisabledImage();
		virtual void SetDisabledImage(LPCTSTR pStrImage);
		virtual LPCTSTR GetHotForeImage();
		virtual void SetHotForeImage(LPCTSTR pStrImage);
		void SetStateCount(int nCount);
		int GetStateCount() const;
		virtual LPCTSTR GetStateImage();
		virtual void SetStateImage(LPCTSTR pStrImage);

		void BindTabIndex(int _BindTabIndex);
		void BindTabLayoutName(LPCTSTR _TabLayoutName);
		void BindTriggerTabSel(int _SetSelectIndex = -1);
		void RemoveBindTabIndex();
		int	 GetBindTabLayoutIndex();
		LPCTSTR GetBindTabLayoutName();

		void SetHotFont(int index);
		int GetHotFont() const;
		void SetPushedFont(int index);
		int GetPushedFont() const;
		void SetFocusedFont(int index);
		int GetFocusedFont() const;

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetPushedBkColor(DWORD dwColor);
		DWORD GetPushedBkColor() const;
		void SetDisabledBkColor(DWORD dwColor);
		DWORD GetDisabledBkColor() const;
		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		SIZE EstimateSize(const SIZE & szAvailable);

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void PaintText(HDC hDC);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);


		// |pstrText| = convenient string to set the type, only the two three chars are used :
		//	"P"==BS_PUSHBUTTON, "push"==BS_PUSHBUTTON, "C"==BS_CHECKBOX, "_CHECK"==BS_AUTOCHECKBOX,
		//	BS_DEFPUSHBUTTON, BS_[AUTO]RADIOBUTTON, BS_[AUTO]3STATE, BS_GROUPBOX, BS_OWNERDRAW...
		// |type| = Button Control Styles. (BS_PUSHBUTTON, ...)
		//	see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
		void SetType(LPCTSTR pstrText=0, int type=BS_OWNERDRAW);

		CDuiString & CButtonUI::GetNote() { 
			return _note; 
		}

		void SetNote(LPCTSTR pstrText) {
			_note = pstrText;
			Invalidate();
		}

		void SetHotTack(bool hotTrack) {
			_hotTrack = hotTrack;
			Invalidate();
		}
	protected:
		UINT m_uButtonState;

		int		m_iHotFont;
		int		m_iPushedFont;
		int		m_iFocusedFont;

		DWORD m_dwHotBkColor;
		DWORD m_dwPushedBkColor;
		DWORD m_dwDisabledBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sHotForeImage;
		CDuiString m_sPushedImage;
		CDuiString m_sPushedForeImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		int m_nStateCount;
		CDuiString m_sStateImage;

		int			m_iBindTabIndex;
		CDuiString	m_sBindTabLayoutName;

		_BUTTON_INFO* infoPtr;
		CDuiString _note;
		bool _hotTrack;
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__