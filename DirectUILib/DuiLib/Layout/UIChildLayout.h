#ifdef QkUIChildLayout

#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CChildLayoutUI : public CContainerUI
	{
		DECLARE_QKCONTROL(CChildLayoutUI)
	public:
		CChildLayoutUI();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildLayoutXML(QkString pXML);
		QkString GetChildLayoutXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;

	private:
		QkString m_pstrXMLFile;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__


#endif