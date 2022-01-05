#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#pragma once

namespace DuiLib {

	class IDialogBuilderCallback
	{
	public:
		virtual CControlUI* CreateControl(LPCTSTR pstrClass) = 0;
	};


	class UILIB_API CDialogBuilder
	{
	public:
		CDialogBuilder();
		CControlUI* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
			CPaintManagerUI* pManager = NULL, CControlUI* pParent = NULL, bool copy=true);
		CControlUI* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManagerUI* pManager = NULL,
			CControlUI* pParent = NULL);

		XMarkupParser* GetMarkup();

		const QkString & GetLastErrorMessage() const;
		const QkString & GetLastErrorLocation() const;
	    void SetInstance(HINSTANCE instance){ m_instance = instance;};

		XMarkupParser m_xml;
	private:
		CControlUI* _Parse(XMarkupNode* parent, QkString & tagNameBuffer, CControlUI* pParent, CPaintManagerUI* pManager);

		IDialogBuilderCallback* m_pCallback;
		LPCTSTR m_pstrtype;
		HINSTANCE m_instance;
	};

} // namespace DuiLib

#endif // __UIDLGBUILDER_H__
