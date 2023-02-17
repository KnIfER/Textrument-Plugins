#ifdef QkUISlider

#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CSliderUI : public CProgressUI
	{
		DECLARE_QKCONTROL(CSliderUI)
	public:
		CSliderUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintForeImage(HDC hDC);

		void SetValue(int nValue);
		void SetCanSendMove(bool bCanSend);
		bool GetCanSendMove() const;
	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;

		QkString m_sThumbImage;
		QkString m_sThumbHotImage;
		QkString m_sThumbPushedImage;

		bool	   m_bSendMove;
	};
}

#endif // __UISLIDER_H__

#endif