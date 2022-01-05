#pragma once
namespace DuiLib {

	typedef struct tagTListInfoUI
	{
		int nColumns;
		int nFont;
		UINT uTextStyle;
		RECT rcTextPadding;
		DWORD dwTextColor;
		DWORD dwBkColor;
		QkString sBkImage;
		bool bAlternateBk;
		DWORD dwSelectedTextColor;
		DWORD dwSelectedBkColor;
		QkString sSelectedImage;
		DWORD dwHotTextColor;
		DWORD dwHotBkColor;
		QkString sHotImage;
		DWORD dwDisabledTextColor;
		DWORD dwDisabledBkColor;
		QkString sDisabledImage;
		DWORD dwLineColor;
		bool bShowRowLine;
		bool bShowColumnLine;
		bool bShowHtml;
		bool bMultiExpandable;
		bool bRSelected;
		CListHeaderUI* headerView;
	} TListInfoUI;

	class IListCallbackUI
	{
	public:
		virtual LPCTSTR GetItemText(CControlUI* pList, int iItem, int iSubItem) = 0;
		virtual DWORD GetItemTextColor(CControlUI* pList, int iItem, int iSubItem, int iState) = 0;// iState：0-正常、1-激活、2-选择、3-禁用
	};

	class IListItemUI
	{
	public:
		virtual int GetIndex() const = 0;
		virtual void SetIndex(int iIndex) = 0;
		virtual CControlUI* GetOwner() = 0;
		virtual void SetOwner(CControlUI* pOwner) = 0;
		virtual bool IsExpanded() const = 0;
		virtual bool Expand(bool bExpand = true) = 0;
		virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
	};

	class UILIB_API CListElementUI : public CControlUI, public IListItemUI
	{
	public:
		CListElementUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetIndex() const;
		void SetIndex(int iIndex);

		CControlUI* GetOwner(){ return m_pOwner?m_pOwner:_parent; };
		void SetOwner(CControlUI* pOwner);
		void SetVisible(bool bVisible = true);

		bool IsExpanded() const;
		bool Expand(bool bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		bool Activate();

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DrawItemBk(HDC hDC, const RECT& rcItem);

	protected:
		int m_iIndex;
		bool m_bSelected;
		UINT m_uButtonState;
		CControlUI* m_pOwner;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CListLabelElementUI : public CListElementUI
	{
		DECLARE_QKCONTROL(CListLabelElementUI)
	public:
		CListLabelElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(const SIZE & szAvailable);
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void DrawItemText(HDC hDC, const RECT& rcItem);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CListTextElementUI : public CListLabelElementUI
	{
		DECLARE_QKCONTROL(CListTextElementUI)
	public:
		CListTextElementUI();
		~CListTextElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		LPCTSTR GetText(int iIndex) const;
		void SetText(int iIndex, LPCTSTR pstrText);

		DWORD GetTextColor(int iIndex) const;
		void SetTextColor(int iIndex, DWORD dwTextColor);

		void SetOwner(CControlUI* pOwner);
		QkString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(const SIZE & szAvailable);

		void DrawItemText(HDC hDC, const RECT& rcItem);

		IListCallbackUI* GetTextCallback() const;
		void SetTextCallback(IListCallbackUI* pCallback);
	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		QkString m_sLinks[MAX_LINK];
		int m_nHoverLink;
		CControlUI* m_pOwner;
		CStdPtrArray m_aTexts;
		CStdPtrArray m_aTextColors;
		IListCallbackUI* m_pCallback;
	};
}

