#ifdef QkUIButton

#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

struct _BUTTON_INFO;

namespace DuiLib
{
	class UILIB_API Button : public CLabelUI
	{
		DECLARE_QKCONTROL(Button)

	public:
		Button();
		~Button();
		CControlUI* Duplicate();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void Init() override;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		int Toggle();
		int GetCheckedValue();

		virtual void AddStatusImage(LPCTSTR pStrImage, int state, bool reset=false);

		virtual TDrawInfo & GetStateImage();
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

		void SyncColors();
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void PaintText(HDC hDC);

		virtual void GetBkFillColor(DWORD & color);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);


		// |handyStr| = convenient string to set the type, only the two three chars are used :
		//	"P"==BS_PUSHBUTTON, "push"==BS_PUSHBUTTON, "C"==BS_CHECKBOX, "_CHECK"==BS_AUTOCHECKBOX,
		//	BS_DEFPUSHBUTTON, BS_[AUTO]RADIOBUTTON, BS_[AUTO]3STATE, BS_GROUPBOX, BS_OWNERDRAW...
		// |type| = Button Control Styles. (BS_PUSHBUTTON, ...)
		//	see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
		void SetType(LPCTSTR handyStr=0, int type=BS_OWNERDRAW);
		int GetType();

		QkString & GetNote() { 
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
		int		m_iHotFont;
		int		m_iPushedFont;
		int		m_iFocusedFont;

		DWORD m_dwHotBkColor;
		DWORD m_dwPushedBkColor;
		DWORD m_dwDisabledBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

		DWORD _focusedBkColor;
		DWORD _focusedTextColor;

		//QkString m_sNormalImage;
		//QkString m_sHotImage;
		//QkString m_sHotForeImage;
		//QkString m_sPushedImage;
		//QkString m_sPushedForeImage;

		//QkString m_sFocusedImage;
		//QkString m_sDisabledImage;

		int m_nStateCount;
		TDrawInfo _stateIcon;
		TDrawInfo _foreIcon;
		IDrawable* btnForeDrawable;

		int			m_iBindTabIndex;
		QkString	m_sBindTabLayoutName;

		_BUTTON_INFO* infoPtr;
		QkString _note;
		bool _hotTrack;
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__

#endif