#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CScrollBarUI;

	class UILIB_API CContainerUI : public CControlUI
	{
		DECLARE_QKCONTROL(CContainerUI)

	public:
		CContainerUI();
		virtual ~CContainerUI();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnabled);
		void SetMouseEnabled(bool bEnable = true);

		virtual int GetChildPadding() const;
		virtual void SetChildPadding(int iPadding);
		virtual void SetClipChildren(bool value, LPCTSTR handyStr);
		virtual UINT GetChildAlign() const;
		virtual void SetChildAlign(UINT iAlign);
		virtual UINT GetChildVAlign() const;
		virtual void SetChildVAlign(UINT iVAlign);
		virtual bool IsMouseChildEnabled() const;
		virtual void SetMouseChildEnabled(bool bEnable = true);

		virtual int FindSelectable(int iIndex, bool bForward = true) const;

		RECT GetClientPos() const;
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true, bool setChild = true);
		CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

		bool SetSubControlText(LPCTSTR pstrSubControlName,LPCTSTR pstrText);
		bool SetSubControlFixedHeight(LPCTSTR pstrSubControlName,int cy);
		bool SetSubControlFixedWdith(LPCTSTR pstrSubControlName,int cx);
		bool SetSubControlUserData(LPCTSTR pstrSubControlName,LPCTSTR pstrText);

		QkString GetSubControlText(LPCTSTR pstrSubControlName);
		int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
		int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
		const QkString GetSubControlUserData(LPCTSTR pstrSubControlName);
		CControlUI* FindSubControl(LPCTSTR pstrSubControlName);

		virtual SIZE GetScrollPos() const;
		virtual SIZE GetScrollRange() const;
		virtual bool SetScrollPos(SIZE szPos, bool bMsg = true, bool seeking = false);
		virtual void DoScroll(int x, int y);
		virtual void SetScrollStepSize(int nSize);
		virtual int GetScrollStepSize() const;
		virtual bool LineUp();
		virtual bool LineDown();
		virtual bool LineLeft();
		virtual bool LineRight();
		virtual void PageUp();
		virtual void PageDown();
		virtual void HomeUp();
		virtual void EndDown();
		virtual void PageLeft();
		virtual void PageRight();
		virtual void HomeLeft();
		virtual void EndRight();
		virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
		virtual CScrollBarUI* GetVerticalScrollBar() const;
		virtual CScrollBarUI* GetHorizontalScrollBar() const;
		
		CScrollBarUI* GetScrollbar(boolean vertical=true) {
			return vertical?m_pVerticalScrollBar:m_pHorizontalScrollBar;
		}

		void SupressChildLayouts(bool value) {
			_bSupressingChildLayout = value;
		}

		bool _bUseSmoothScroll;
		bool _bSnapFirstRow;

	protected:
		virtual void SetFloatPos(int iIndex);
		virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

	protected:
		int m_iChildPadding;
		bool _clipchildren;
		UINT m_iChildAlign;
		UINT m_iChildVAlign;
		bool m_bMouseChildEnabled;
		int	 m_nScrollStepSize;

		int scrollbars_set_cnt;

		CScrollBarUI* m_pVerticalScrollBar;
		CScrollBarUI* m_pHorizontalScrollBar;
		QkString	m_sVerticalScrollBarStyle;
		QkString	m_sHorizontalScrollBarStyle;


		int _seeking=false;
		int _scrollSpeed;
		int _scrollTarget;
		bool _smoothScrolling;
		int _scrollY;
		int _scrollX;

		bool _vscrollEatInset;
		bool _hscrollEatInset;

		bool _bSupressingChildLayout;
	};

} // namespace DuiLib

#endif // __UICONTAINER_H__
