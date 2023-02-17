#ifdef QkUIActiveX

#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;


namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CActiveXCtrl;

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
	{
		DECLARE_QKCONTROL(CActiveXUI)

		friend class CActiveXCtrl;
	public:
		CActiveXUI();
		virtual ~CActiveXUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		HWND GetHostWindow() const;

		virtual bool IsDelayCreate() const;
		virtual void SetDelayCreate(bool bDelayCreate = true);
		virtual bool IsMFC() const;
		virtual void SetMFC(bool bMFC = false);

		bool CreateControl(const CLSID clsid);
		bool CreateControl(LPCTSTR pstrCLSID);
		HRESULT GetControl(const IID iid, LPVOID* ppRet);
		CLSID GetClisd() const;
		QkString GetModuleName() const;
		void SetModuleName(LPCTSTR pstrText);

		void SetVisible(bool bVisible = true);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	protected:
		virtual void ReleaseControl();
		virtual bool DoCreateControl();

	protected:
		CLSID m_clsid;
		QkString m_sModuleName;
		bool m_bCreated;
		bool m_bDelayCreate;
		bool m_bMFC;
		IOleObject* m_pUnk;
		CActiveXCtrl* m_pControl;
		HWND m_hwndHost;
	};

} // namespace DuiLib

#endif // __UIACTIVEX_H__


#endif