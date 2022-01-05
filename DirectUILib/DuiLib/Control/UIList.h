#ifndef __UILIST_H__
#define __UILIST_H__

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class IListOwnerUI
	{
	public:
		virtual UINT GetListType() = 0;
		//virtual TListInfoUI* GetListInfo() = 0;
		virtual int GetCurSel() const = 0;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false) = 0;
		virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) = 0;
		virtual bool UnSelectItem(int iIndex, bool bOthers = false) = 0;
		virtual void DoEvent(TEventUI& event) = 0;
	};

	class IListUI : public IListOwnerUI
	{
	public:
		virtual CListHeaderUI* GetHeader() const = 0;
		virtual CContainerUI* GetList() const = 0;
		virtual IListCallbackUI* GetTextCallback() const = 0;
		virtual void SetTextCallback(IListCallbackUI* pCallback) = 0;
		virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
		virtual int GetExpandedItem() const = 0;

		virtual void SetMultiSelect(bool bMultiSel) = 0;
		virtual bool IsMultiSelect() const = 0;
		virtual void SelectAllItems() = 0;
		virtual void UnSelectAllItems() = 0;
		virtual int GetSelectItemCount() const = 0;
		virtual int GetNextSelItem(int nItem) const = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CListBodyUI;
	class CListHeaderUI;
	class CEditUI;
	class CComboBoxUI;
	class UILIB_API CListUI : public CVerticalLayoutUI, public IListUI
	{
		DECLARE_QKCONTROL(CListUI)

	public:
		CListUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool GetScrollSelect();
		void SetScrollSelect(bool bScrollSelect);
		int GetCurSel() const;
		int GetCurSelActivate() const;
		bool SelectItem(int iIndex, bool bTakeFocus = false);
		bool SelectItemActivate(int iIndex);    // 双击选中

		bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
		void SetMultiSelect(bool bMultiSel);
		bool IsMultiSelect() const;
		bool UnSelectItem(int iIndex, bool bOthers = false);
		void SelectAllItems();
		void UnSelectAllItems();
		int GetSelectItemCount() const;
		int GetNextSelItem(int nItem) const;

		bool IsFixedScrollbar();
		void SetFixedScrollbar(bool bFixed);

		CListHeaderUI* GetHeader() const;  
		CContainerUI* GetList() const;
		UINT GetListType();
		LRESULT GetAttribute(LPCTSTR pstrName, LPARAM lParam=0, WPARAM wParam=0);

		CControlUI* GetItemAt(int iIndex) const;
		int GetItemIndex(CControlUI* pControl) const;
		bool SetItemIndex(CControlUI* pControl, int iIndex);
		int GetCount() const;
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

		bool IsDelayedDestroy() const;
		void SetDelayedDestroy(bool bDelayed);
		int GetChildPadding() const;
		void SetChildPadding(int iPadding);

		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		void SetItemTextPadding(RECT rc);
		void SetItemTextColor(DWORD dwTextColor);
		void SetItemBkColor(DWORD dwBkColor);
		void SetItemBkImage(LPCTSTR pStrImage);
		void SetAlternateBk(bool bAlternateBk);
		void SetSelectedItemTextColor(DWORD dwTextColor);
		void SetSelectedItemBkColor(DWORD dwBkColor);
		void SetSelectedItemImage(LPCTSTR pStrImage); 
		void SetHotItemTextColor(DWORD dwTextColor);
		void SetHotItemBkColor(DWORD dwBkColor);
		void SetHotItemImage(LPCTSTR pStrImage);
		void SetDisabledItemTextColor(DWORD dwTextColor);
		void SetDisabledItemBkColor(DWORD dwBkColor);
		void SetDisabledItemImage(LPCTSTR pStrImage);
		void SetItemLineColor(DWORD dwLineColor);
		void SetItemShowRowLine(bool bShowLine = false);
		void SetItemShowColumnLine(bool bShowLine = false);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);
		bool IsItemRSelected();
		void SetItemRSelected(bool bSelected = true);
		RECT GetItemTextPadding() const;
		DWORD GetItemTextColor() const;
		DWORD GetItemBkColor() const;
		LPCTSTR GetItemBkImage() const;
		bool IsAlternateBk() const;
		DWORD GetSelectedItemTextColor() const;
		DWORD GetSelectedItemBkColor() const;
		LPCTSTR GetSelectedItemImage() const;
		DWORD GetHotItemTextColor() const;
		DWORD GetHotItemBkColor() const;
		LPCTSTR GetHotItemImage() const;
		DWORD GetDisabledItemTextColor() const;
		DWORD GetDisabledItemBkColor() const;
		LPCTSTR GetDisabledItemImage() const;
		DWORD GetItemLineColor() const;

		void SetMultiExpanding(bool bMultiExpandable); 
		int GetExpandedItem() const;
		bool ExpandItem(int iIndex, bool bExpand = true);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		IListCallbackUI* GetTextCallback() const;
		void SetTextCallback(IListCallbackUI* pCallback);

		SIZE GetScrollPos() const;
		SIZE GetScrollRange() const;
		bool SetScrollPos(SIZE szPos, bool bMsg = true);
		bool LineUp();
		bool LineDown();
		bool LineLeft();
		bool LineRight();
		void PageUp();
		void PageDown();
		void HomeUp();
		void EndDown();
		void PageLeft();
		void PageRight();
		void HomeLeft();
		void EndRight();
		void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
		virtual CScrollBarUI* GetVerticalScrollBar() const;
		virtual CScrollBarUI* GetHorizontalScrollBar() const;
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

		virtual BOOL CheckColumEditable(int nColum) { return FALSE; };
		virtual CRichEditUI* GetEditUI() { return NULL; };
		virtual BOOL CheckColumComboBoxable(int nColum) { return FALSE; };
		virtual CComboBoxUI* GetComboBoxUI() { return NULL; };

	protected:
		int GetMinSelItemIndex();
		int GetMaxSelItemIndex();

	protected:
		bool m_bFixedScrollbar;
		bool m_bScrollSelect;
		bool m_bMultiSel;
		int m_iCurSel;
		int m_iFirstSel;
		CStdPtrArray m_aSelItems;
		int m_iCurSelActivate;  // 双击的列
		int m_iExpandedItem;
		IListCallbackUI* m_pCallback;
		CListBodyUI* m_pList;
		CListHeaderUI* m_pHeader;
		TListInfoUI m_ListInfo;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CListBodyUI : public CVerticalLayoutUI
	{
	public:
		CListBodyUI(CListUI* pOwner);


		int GetScrollStepSize() const;
		bool SetScrollPos(SIZE szPos, bool bMsg = true);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);
	protected:
		static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
		int __cdecl ItemComareFunc(const void *item1, const void *item2);
	protected:
		CListUI* m_pOwner;
		PULVCompareFunc m_pCompareFunc;
		UINT_PTR m_compareData;
	};

	class UILIB_API CListContainerElementUI : public CHorizontalLayoutUI, public IListItemUI
	{
		DECLARE_QKCONTROL(CListContainerElementUI)
	public:
		CListContainerElementUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		int GetIndex() const;
		void SetIndex(int iIndex);

		CControlUI* GetOwner();
		void SetOwner(CControlUI* pOwner);
		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);

		bool IsExpanded() const;
		bool Expand(bool bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		bool Activate();

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		virtual void DrawItemText(HDC hDC, const RECT& rcItem);    
		virtual void DrawItemBk(HDC hDC, const RECT& rcItem);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

	protected:
		int m_iIndex;
		bool m_bSelected;
		UINT m_uButtonState;
		CControlUI* m_pOwner;
	};

} // namespace DuiLib

#endif // __UILIST_H__
