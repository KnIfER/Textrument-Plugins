#ifndef __UIROTATE_H__
#define __UIROTATE_H__

#pragma once

namespace DuiLib
{
	class CRingUI : public CLabelUI
	{
		enum
		{ 
			RING_TIMERID = 100,
		};
	public:
		static CControlUI* CreateControl();
	public:
		CRingUI();
		~CRingUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetBkImage(LPCTSTR pStrImage);	
		virtual void DoEvent(TEventUI& event);
		virtual void PaintBkImage(HDC hDC);	

	private:
		void InitImage();
		void DeleteImage();

	public:
		float m_fCurAngle;
		Gdiplus::Image* m_pBkimage;
	};
}

#endif // __UIROTATE_H__