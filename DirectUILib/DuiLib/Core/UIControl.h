#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class IContainerUI
	{
	public:
		virtual CControlUI* GetItemAt(int iIndex) const = 0;
		virtual int GetItemIndex(CControlUI* pControl) const  = 0;
		virtual bool SetItemIndex(CControlUI* pControl, int iIndex)  = 0;
		virtual int GetCount() const = 0;
		virtual bool Add(CControlUI* pControl) = 0;
		virtual bool AddAt(CControlUI* pControl, int iIndex)  = 0;
		virtual bool Remove(CControlUI* pControl) = 0;
		virtual bool RemoveAt(int iIndex)  = 0;
		virtual void RemoveAll() = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

	class CContainerUI;

	class UILIB_API CControlUI : public IContainerUI
	{
		DECLARE_QKCONTROL(CControlUI)
	public:
		CControlUI();
		virtual ~CControlUI();

	public:
		virtual QkString GetName() const;
		virtual void SetName(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;

		virtual bool Activate();
		virtual CPaintManagerUI* GetManager() const;
		virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true, bool setChild = true);
		virtual CControlUI* GetParent() const;
	    void setInstance(HINSTANCE instance = NULL);
		HWND GetHWND();

		// 子布局
		CControlUI* GetItemAt(int iIndex) const {
			return iIndex < 0 || iIndex >= m_items.GetSize()?NULL:static_cast<CControlUI*>(m_items[iIndex]);
		}
		int GetItemIndex(CControlUI* pControl) const;
		bool SetItemIndex(CControlUI* pControl, int iIndex);
		int GetCount() const {
			return m_items.GetSize();
		};
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();
		bool IsAutoDestroy() const;
		void SetAutoDestroy(bool bAuto);
		bool IsDelayedDestroy() const;
		void SetDelayedDestroy(bool bDelayed);
		virtual CControlUI* Duplicate();
		bool PaintChildren(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		const RECT & GetInset() const {
			if(_manager) return _manager->GetDPIObj()->Scale(m_rcInset);
			return m_rcInset;
		}
		void ApplyInsetToRect(RECT & rc) const;
		void ReAddInsetToRect(RECT & rc) const;
		void ApplyBorderInsetToRect(RECT & rc) const;
		// 设置内边距，相当于安卓中的Padding
		void SetInset(const RECT & rcInset, LPCTSTR handyStr = 0);

		// 定时器
		bool SetTimer(UINT nTimerID, UINT nElapse, bool restart=false);
		void KillTimer(UINT nTimerID);

		// 文本相关
		virtual QkString & GetText();
		virtual void SetText(LPCTSTR pstrText);
		int GetFont() const {
			return _font;
		}
		void SetFont(LPCTSTR pstrValue, int parsedId=-1);

		virtual bool IsResourceText() const;
		virtual void SetResourceText(bool bResource);

		virtual bool IsDragEnabled() const;
		virtual void SetDragEnable(bool bDrag);

		virtual bool IsDropEnabled() const;
		virtual void SetDropEnable(bool bDrop);

		bool IsRichEvent() const; // Notify Mouse Enter, Mouse Leave, Button Down.
		void SetRichEvent(bool bEnable); // Notify Mouse Enter, Mouse Leave, Button Down.

		// 图形相关
		LPCTSTR GetGradient();
		void SetGradient(LPCTSTR pStrImage);
		DWORD GetBkColor() const;
		void SetBkColor(DWORD dwBackColor);
		DWORD GetBkColor2() const;
		void SetBkColor2(DWORD dwBackColor);
		DWORD GetBkColor3() const;
		void SetBkColor3(DWORD dwBackColor);
		DWORD GetForeColor() const;
		void SetForeColor(DWORD dwForeColor);
		LPCTSTR GetBkImage();
		void SetBkImage(LPCTSTR pStrImage);
		LPCTSTR GetForeImage() const;
		void SetForeImage(LPCTSTR pStrImage);

		bool IsColorHSL() const;
		void SetColorHSL(bool bColorHSL);
		bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

		// 是否裁切出圆角内容
		void SetRoundClip(bool bClip);
		void SetBkRound(bool val);

		//边框相关
		SIZE GetBorderRound() const;
		void SetBorderRound(SIZE cxyRound, LPCTSTR handyStr=0);
		// 指示均匀粗细的圆角边框的绘制方式：0=自动，边框过粗时绘制空心矩形；1=强制绘制路径; 2=强制绘制空心矩形
		void SetBorderArc(int val);
		//int GetBorderRoundForceEvenType();
		// 高亮加粗
		void SetBorderEnhanced(bool val);	
		bool GetBorderEnhanced();	

		//边框相关
		int GetBorderSize() const;
		void SetBorderSize(int nSize);

		DWORD GetBorderColor() const;
		void SetBorderColor(DWORD dwBorderColor);
		DWORD GetFocusBorderColor() const;
		void SetFocusBorderColor(DWORD dwBorderColor);

		void SetBorderSize(const RECT & rc, LPCTSTR handyStr = 0);
		int GetLeftBorderSize() const;
		void SetLeftBorderSize(int nSize);
		int GetTopBorderSize() const;
		void SetTopBorderSize(int nSize);
		int GetRightBorderSize() const;
		void SetRightBorderSize(int nSize);
		int GetBottomBorderSize() const;
		void SetBottomBorderSize(int nSize);
		int GetBorderStyle() const;
		void SetBorderStyle(int nStyle);
		void SetBorderInset(const RECT & rcInset, LPCTSTR handyStr = 0);

		// 位置相关
		virtual RECT GetRelativePos() const; // 相对(父控件)位置
		virtual RECT GetAbsolutePos() const; // 窗口绝对位置
		virtual RECT GetClientPos() const; // 客户区域（除去scrollbar和inset）
		virtual const RECT& GetPos() const;
		virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
		virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
		virtual int GetWidth() const;
		virtual int GetHeight() const;
		virtual int GetX() const;
		virtual int GetY() const;
		virtual RECT GetPadding() const;
		virtual void SetPadding(RECT rcPadding, LPCTSTR handyStr = 0); // 设置外边距，由上层窗口绘制
		virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
		virtual SIZE GetFixedSize() const;
		virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedWidth(int cx);      // 预设的参考值
		virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedHeight(int cy);     // 预设的参考值
		virtual int GetMinWidth() const;
		virtual void SetMinWidth(int cx);
		virtual int GetMaxWidth() const;
		virtual void SetMaxWidth(int cx);
		virtual int GetMinHeight() const;
		virtual void SetMinHeight(int cy);
		virtual int GetMaxHeight() const;
		virtual void SetMaxHeight(int cy);
		virtual int GetMaxAvailWidth() const;
		virtual int GetMaxAvailHeight() const;
		virtual TPercentInfo GetFloatPercent() const;
		virtual void SetFloatPercent(TPercentInfo piFloatPercent);
		virtual void SetFloatAlign(UINT uAlign);
		virtual UINT GetFloatAlign() const;
		// 鼠标提示
		virtual QkString GetToolTip() const;
		virtual void SetToolTip(LPCTSTR pstrText);
		virtual void SetToolTipWidth(int nWidth);
		virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度
		
		// 光标
		virtual WORD GetCursor();
		virtual void SetCursor(WORD wCursor);

		// 快捷键
		virtual TCHAR GetShortcut() const;
		virtual void SetShortcut(TCHAR ch);

		// 菜单
		virtual bool IsContextMenuUsed() const;
		virtual void SetContextMenuUsed(bool bMenuUsed);

		// 用户属性
		virtual const QkString& GetUserData(); // 辅助函数，供用户使用
		virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
		virtual void SetUserDataTranslator(LPCTSTR pstrText); // 辅助函数，供用户使用
		virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
		virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

		// 一些重要的属性 todo 去除虚拟化
		void SetDirectUI(bool value);
		bool IsDirectUI() const;
		virtual bool IsVisible() const;
		virtual void SetVisible(bool bVisible = true);
		virtual bool IsEnabled() const;
		virtual void SetEnabled(bool bEnable = true);
		virtual bool IsMouseEnabled() const;
		virtual void SetMouseEnabled(bool bEnable = true);
		virtual bool IsKeyboardEnabled() const;
		virtual void SetKeyboardEnabled(bool bEnable = true);
		virtual bool IsFocused() const;
		virtual void SetFocus();
		virtual void StatFocus();
		virtual bool HasFocus() const;
		virtual bool IsFloat() const;
		virtual void SetFloat(bool bFloat = true);

		virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

		virtual void resize(){};

		CContainerUI* GetRoot();

		// Redraw the control region if visible. 
		void Invalidate();
		// Is marked to relayout? 
		bool IsUpdateNeeded() const;
		// Request update/relayout and redraw. 
		void NeedUpdate();
		// Request parent update
		void NeedParentUpdate();
		// Request parent update. If the parent's measurement depends on the children,
		//	Propagate to ascendents. see RichEdit.
		void NeedParentAutoUpdate();
		bool GetAutoMeasureDimensionMatch(CControlUI* other) {
			return m_bAutoCalcHeight && other->GetAutoCalcHeight() || m_bAutoCalcWidth && other->GetAutoCalcWidth();
		};

		DWORD GetAdjustColor(DWORD dwColor);

		virtual void Init();
		virtual void DoInit();

		virtual void Event(TEventUI& event);
		virtual void DoEvent(TEventUI& event);

		// 自定义(未处理的)属性
		void AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
		LPCTSTR GetCustomAttribute(LPCTSTR pstrName) const;
		bool RemoveCustomAttribute(LPCTSTR pstrName);
		void RemoveAllCustomAttribute();

		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual LRESULT GetAttribute(LPCTSTR pstrName, LPARAM lParam=0, WPARAM wParam=0);
		CControlUI* ApplyAttributeList(LPCTSTR pstrList);
		CControlUI* ApplyAttributeList(Style* pstrList);

		virtual SIZE EstimateSize(const SIZE & szAvailable);
		virtual void OnDPIChanged();
		virtual bool Paint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl = NULL); // 返回要不要继续绘制
		virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		virtual void PaintBkColor(HDC hDC);
		virtual void GetBkFillColor(DWORD & color){};
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintForeColor(HDC hDC);
		virtual void PaintForeImage(HDC hDC);
		virtual void PaintText(HDC hDC);
		virtual void PaintBorder(HDC hDC);

		virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

		void SetNeedAutoCalcSize();

		//虚拟窗口参数
		void SetVirtualWnd(LPCTSTR pstrValue);
		QkString GetVirtualWnd() const;

		// 动画特效
		bool GetEffectEnabled(int fx_section);
		std::vector<TEffectAge> & GetEffects();
		TEffectAge & GetCurrentEffect();
		TEffectAge* GetMouseInEffect();
		TEffectAge* GetMouseOutEffect();
		TEffectAge* GetClickInEffect();

		virtual QkString GetEffectStyle();
		virtual void SetAnimEffects(bool bEnableEffect);
		virtual bool GetAnimEffects();
		virtual void SetEffectsZoom(int iZoom);
		virtual int  GetEffectsZoom();
		virtual void SetEffectsFillingBK(DWORD dFillingBK);
		virtual DWORD GetEffectsFillingBK();
		virtual void SetEffectsOffectX(int iOffectX);
		virtual int  GetEffectsOffectX();
		virtual void SetEffectsOffectY(int iOffectY);
		virtual int  GetEffectsOffectY();
		virtual void SetEffectsAlpha(int iAlpha);
		virtual int  GetEffectsAlpha();
		virtual void SetEffectsRotation(float fRotation);
		virtual float GetEffectsRotation();
		virtual void SetEffectsNeedTimer(int iNeedTimer);
		virtual int  GetEffectsNeedTimer();

		virtual void TriggerEffects(TEffectAge* pTEffectAge = NULL);
		virtual void SetEffectsStyle(LPCTSTR pstrEffectStyle,TEffectAge* pTEffectAge = NULL);
		void AnyEffectsAdvProfiles(LPCTSTR pstrEffects,TEffectAge* pTEffectAge = NULL);
		void AnyEasyEffectsPorfiles(LPCTSTR pstrEffects,TEffectAge* pTEffectAge = NULL);

		bool GetAutoCalcWidth() const {
			return m_bAutoCalcWidth;
		}
		void SetAutoCalcWidth(bool bAutoCalcWidth) {
			m_cxyFixed.cx = bAutoCalcWidth?-2:-1;
		}
		bool GetAutoCalcHeight() const {
			return m_bAutoCalcHeight;
		}
		void SetAutoCalcHeight(bool bAutoCalcHeight) {
			m_cxyFixed.cy = bAutoCalcHeight?-2:-1;
		}
	public:
		CEventSource OnInit;
		CEventSource OnDestroy;
		CEventSource OnSize;
		CEventSource OnEvent;
		CEventSource OnNotify;
		char* m_sUserDataTally;
		int _marked;
	protected:
		CPaintManagerUI* _manager;
		CControlUI* _parent;
		int _view_states;
		int _LastScaleProfile;

		QkString m_sName;
		CStdPtrArray m_items;

		HWND _hWnd;
		HWND _hParent;
		HINSTANCE _instance;

		QkString m_sVirtualWnd;

		RECT m_rcInset;
		RECT m_rcInsetScaled;
		SIZE m_cxyFixScaled;

		RECT m_rcItem;
		RECT m_rcPadding;
		SIZE m_cXY;
		SIZE m_cxyFixed;
		SIZE m_cxyMin;
		SIZE m_cxyMax;

		TPercentInfo m_piFloatPercent;
		UINT m_uFloatAlign;
		TRelativePosUI m_tRelativePos;

		//bool	m_bAutoCalcWidth;
		//bool	m_bAutoCalcHeight;

		SIZE    m_szAvailableLast;
		SIZE    m_cxyFixedLast;


		//bool _focusable;
		//bool m_bMouseEnabled;
		//bool m_bKeyboardEnabled;
		//bool m_bMenuUsed;
		//bool m_bDragEnabled;
		//bool m_bDropEnabled;
		//bool m_bRichEvent;
		// 
		//bool m_bSettingPos;
		//bool m_bNeedEstimateSize;
		//
		//bool m_bIsDirectUI = true;
		//bool m_bIsViewGroup;
		//bool m_bAutoDestroy;
		//bool m_bDelayedDestroy;
		//bool m_bUpdateNeeded;
		//bool m_bEnabledEffect;
		//
		//bool m_bFloat;
		//bool m_bResourceText;
		//bool m_bColorHSL;
		//bool m_bRoundClip;


		int		_font;
		QkString m_sStyleName;
		QkString m_sText;
		QkString m_sToolTip;
		TCHAR m_chShortcut;
		QkString m_sUserData;
		UINT_PTR m_pTag;

		QkString m_sGradient;
		DWORD m_dwBackColor;
		DWORD m_dwBackColor2;
		DWORD m_dwBackColor3;
		DWORD m_dwForeColor;
		QkString m_sBkImage;
		QkString m_sForeImage;

		DWORD m_dwBorderColor;
		DWORD m_dwFocusBorderColor;

		// 0=zero_sized; 1=even_sized; 2=hetero_sized
		BYTE _borderSizeType;
		BYTE _round;
		// ...
		BYTE m_nBorderStyle;
		SIZE m_cxyBorderRound;
		SIZE _sizeBorderRoundScaled;
		RECT m_rcBorderSize;
		RECT _rcBorderSizeScaled;
		RECT _borderInset;
		RECT _borderInsetScaled;

		int m_nTooltipWidth;
		WORD m_wCursor;
		HCURSOR _hCursor;
		RECT m_rcPaint;

		QkStringPtrMap m_mCustomAttrHash;

		QkString	m_strEffectStyle;
		std::vector<TEffectAge> effects;
	};

} // namespace DuiLib

#endif // __UICONTROL_H__
