#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CScrollBarUI : public CControlUI
	{
		DECLARE_QKCONTROL(CScrollBarUI)
	public:
		CScrollBarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		void SetScrollRangeAndPos(int nRange, int nPos);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage();
		void SetBkNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage();
		void SetBkHotImage(LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage();
		void SetBkPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage();
		void SetBkDisabledImage(LPCTSTR pStrImage);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

		void SetMouseBias(int bias) {
			_lastMouseBias = bias;
		}

		LONG GetThumbPosition();

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		__int64 m_nRange;
		__int64 m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT m_ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int _lastMouseBias;
		int m_nScrollRepeatDelay;

		QkString m_sBkNormalImage;
		QkString m_sBkHotImage;
		QkString m_sBkPushedImage;
		QkString m_sBkDisabledImage;

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		QkString m_sButton1NormalImage;
		QkString m_sButton1HotImage;
		QkString m_sButton1PushedImage;
		QkString m_sButton1DisabledImage;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		QkString m_sButton2NormalImage;
		QkString m_sButton2HotImage;
		QkString m_sButton2PushedImage;
		QkString m_sButton2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		QkString m_sThumbNormalImage;
		QkString m_sThumbHotImage;
		QkString m_sThumbPushedImage;
		QkString m_sThumbDisabledImage;

		QkString m_sRailNormalImage;
		QkString m_sRailHotImage;
		QkString m_sRailPushedImage;
		QkString m_sRailDisabledImage;

		TDrawInfo drawableTmp;
	};
}

#endif // __UISCROLLBAR_H__