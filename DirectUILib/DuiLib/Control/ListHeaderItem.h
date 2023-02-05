#pragma once
namespace DuiLib {

	class UILIB_API CListHeaderItemUI : public CContainerUI
	{
		DECLARE_QKCONTROL(CListHeaderItemUI)

	public:
		CListHeaderItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);

		bool IsDragable() const;
		void SetDragable(bool bDragable);
		DWORD GetSepWidth() const;
		void SetSepWidth(int iWidth);
		DWORD GetTextStyle() const;
		void SetTextStyle(UINT uStyle);
		DWORD GetTextColor() const;
		void SetTextColor(DWORD dwTextColor);
		void SetTextPadding(RECT rc);
		RECT GetTextPadding() const;
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);
		LPCTSTR GetNormalImage() const;
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage() const;
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage() const;
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetSepImage() const;
		void SetSepImage(LPCTSTR pStrImage);
		void SetScale(int nScale);
		int GetScale() const;

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(const SIZE & szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		RECT GetThumbRect() const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		POINT ptLastMouse;
		RECT _rcLastPos;
		bool m_bDragable;
		UINT m_uButtonState;
		int m_iSepWidth;
		DWORD m_dwTextColor;
		UINT m_uTextStyle;
		bool m_bShowHtml;
		RECT m_rcTextPadding;
		QkString m_sNormalImage;
		QkString m_sHotImage;
		QkString m_sPushedImage;
		QkString m_sFocusedImage;
		QkString m_sSepImage;
		TDrawInfo drawableTmp;
		int m_nScale;
	};

}

