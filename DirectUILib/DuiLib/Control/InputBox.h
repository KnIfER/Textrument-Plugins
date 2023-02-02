#pragma once

struct _EDIT_STATE;

namespace DuiLib
{
	class UILIB_API InputBox : public CContainerUI
	{
		DECLARE_QKCONTROL(InputBox)
		friend class CEditWnd;
	public:
		InputBox();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetText(LPCTSTR pstrText) override;
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;

		//LPCTSTR GetNormalImage();
		//void SetNormalImage(LPCTSTR pStrImage);
		//LPCTSTR GetHotImage();
		//void SetHotImage(LPCTSTR pStrImage);
		//LPCTSTR GetFocusedImage();
		//void SetFocusedImage(LPCTSTR pStrImage);
		//LPCTSTR GetDisabledImage();
		//void SetDisabledImage(LPCTSTR pStrImage);
		//void SetNativeEditBkColor(DWORD dwBkColor);
		//DWORD GetNativeEditBkColor() const;
		//void SetNativeEditTextColor( LPCTSTR pStrColor );
		//DWORD GetNativeEditTextColor() const;

		//bool IsAutoSelAll();
		//void SetAutoSelAll(bool bAutoSelAll);
		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetTipValue(LPCTSTR pStrTipValue);
		LPCTSTR GetTipValue();
		void SetTipValueColor(LPCTSTR pStrColor);
		DWORD GetTipValueColor();

		//virtual void SetScrollPos(SIZE szPos, bool bMsg = true);
		virtual void DoScroll(int x, int y);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		//void SetVisible(bool bVisible = true);
		//void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(const SIZE & szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetTextColor(DWORD dwTextColor, LPCTSTR handyStr=NULL);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;

		void Init();

		void SyncColors();

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		virtual void PaintText(HDC hDC);

		void ShowCaretIfFocused(bool update);
		void SetVCenter(bool value);
		void SetMultiline(bool value);
	protected:
		RECT _rcEdit;
		RECT _rcEditMax;
		DWORD	m_dwTextColor;
		DWORD	m_dwDisabledTextColor;

		bool _twinkleMark;

		_EDIT_STATE* infoPtr;
		UINT m_uButtonState;
		UINT m_uMaxChar;
		DWORD m_dwTipValueColor;
		BOOL VCENTER; // only for single line text for now.
		INT VCENTER_OFFSET;	
	};
}