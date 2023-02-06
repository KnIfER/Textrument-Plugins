#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib
{
	class UILIB_API OptionBtn : public Button
	{
		DECLARE_QKCONTROL(OptionBtn)
	public:
		OptionBtn();
		~OptionBtn();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true, bool setChild = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);
		LPCTSTR GetSelectedPushedImage();
		void SetSelectedPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetSelectedForedImage();
		void SetSelectedForedImage(LPCTSTR pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();
		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();


		void SetSelectedStateCount(int nCount);
		int GetSelectedStateCount() const;

		virtual LPCTSTR GetSelectedStateImage();
		virtual void SetSelectedStateImage(LPCTSTR pStrImage);

		void SetSelectedFont(int index);
		int GetSelectedFont() const;

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		bool IsSelected() const;
		virtual void Selected(bool bSelected, bool bMsg = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		bool			m_bSelected;
		QkString		m_sGroupName;

		int				m_iSelectedFont;

		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;

		QkString		m_sSelectedImage;
		QkString		m_sSelectedHotImage;
		QkString		m_sSelectedPushedImage;
		QkString		m_sSelectedForeImage;

		int m_nSelectedStateCount;
		QkString m_sSelectedStateImage;
	};

	class UILIB_API CCheckBoxUI : public OptionBtn
	{
		DECLARE_QKCONTROL(CCheckBoxUI)
	public:
		CCheckBoxUI();

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);

		void SetCheck(bool bCheck);
		bool GetCheck() const;

	public:
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetAutoCheck(bool bEnable);
		virtual void DoEvent(TEventUI& event);
		virtual void Selected(bool bSelected, bool bMsg = true);

	protected:
		bool m_bAutoCheck; 
	};
} // namespace DuiLib

#endif // __UIOPTION_H__