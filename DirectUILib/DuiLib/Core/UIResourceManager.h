#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

namespace DuiLib {
	// 控件文字查询接口
	class UILIB_API IQueryControlText
	{
	public:
		virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
	};

	class UILIB_API CResourceManager
	{
	private:
		CResourceManager(void);
		~CResourceManager(void);

	public:
		static CResourceManager* GetInstance()
		{
			static CResourceManager * p = new CResourceManager;
			return p;
		};	
		void Release(void) { delete this; }

	public:
		BOOL LoadResource(STRINGorID xml, LPCTSTR type = NULL);
		BOOL LoadResource(XMarkupNode Root);
		void ResetResourceMap();
		LPCTSTR MapImagePath(LPCTSTR lpstrId);
		LPCTSTR GetXmlPath(LPCTSTR lpstrId);

	public:
		void SetLanguage(LPCTSTR pstrLanguage) { m_sLauguage = pstrLanguage; }
		LPCTSTR GetLanguage() { return m_sLauguage; }
		BOOL LoadLanguage(LPCTSTR pstrXml);
		
	public:
		void SetTextQueryInterface(IQueryControlText* pInterface) { m_pQuerypInterface = pInterface; }
		QkString & GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);
		void ReloadText();
		void ResetTextMap();

	private:
		QkStringPtrMap m_mTextResourceHashMap;
		IQueryControlText*	m_pQuerypInterface;
		QkStringPtrMap m_mImageHashMap;
		QkStringPtrMap m_mXmlHashMap;
		XMarkupParser m_xml;
		QkString m_sLauguage;
		QkStringPtrMap m_mTextHashMap;
	};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__