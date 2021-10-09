#pragma once

struct _EDIT_STATE;

namespace DuiLib
{
	class UILIB_API InputBox : public CControlUI
	{
		DECLARE_QKCONTROL(InputBox)
		friend class CEditWnd;
	public:
		InputBox();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
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
		int GetWindowStyls() const;

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

		HWND GetHWND();

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		//void SetVisible(bool bVisible = true);
		//void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(const SIZE & szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void InputBox::Init();
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void PaintStatusImage(HDC hDC);

		void ShowCaretIfVisible(bool update);
	protected:
		bool _twinkleMark;

		_EDIT_STATE* infoPtr;
		UINT m_uButtonState;
		UINT m_uMaxChar;
		DWORD m_dwTipValueColor;
	};
}