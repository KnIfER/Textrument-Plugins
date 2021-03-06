#include "StdAfx.h"
#include "ControlFactory.h"

namespace DuiLib 
{
	CControlFactory::CControlFactory()
	{
		INNER_REGISTER_DUICONTROL(CControlUI);
		INNER_REGISTER_DUICONTROL(CContainerUI);
		INNER_REGISTER_DUICONTROL(CButtonUI);
		INNER_REGISTER_DUICONTROL(CComboUI);
		INNER_REGISTER_DUICONTROL(CComboBoxUI);
		INNER_REGISTER_DUICONTROL(CDateTimeUI);
		INNER_REGISTER_DUICONTROL(CEditUI);
		INNER_REGISTER_DUICONTROL(CActiveXUI);
		INNER_REGISTER_DUICONTROL(CFlashUI);
		INNER_REGISTER_DUICONTROL(CGifAnimUI);
#ifdef USE_XIMAGE_EFFECT
		INNER_REGISTER_DUICONTROL(CGifAnimExUI);
#endif
		INNER_REGISTER_DUICONTROL(CGroupBoxUI);
		INNER_REGISTER_DUICONTROL(CIPAddressUI);
		INNER_REGISTER_DUICONTROL(CIPAddressExUI);
		INNER_REGISTER_DUICONTROL(CLabelUI);
		INNER_REGISTER_DUICONTROL(CListUI);
		INNER_REGISTER_DUICONTROL(CListHeaderUI);
		INNER_REGISTER_DUICONTROL(CListHeaderItemUI);
		INNER_REGISTER_DUICONTROL(CListLabelElementUI);
		INNER_REGISTER_DUICONTROL(CListTextElementUI);
		INNER_REGISTER_DUICONTROL(CListContainerElementUI);
		INNER_REGISTER_DUICONTROL(CMenuUI);
		INNER_REGISTER_DUICONTROL(CMenuElementUI);
		INNER_REGISTER_DUICONTROL(COptionUI);
		INNER_REGISTER_DUICONTROL(CCheckBoxUI);
		INNER_REGISTER_DUICONTROL(CProgressUI);
		INNER_REGISTER_DUICONTROL(CRichEditUI);
		INNER_REGISTER_DUICONTROL(CScrollBarUI);
		INNER_REGISTER_DUICONTROL(CSliderUI);
		INNER_REGISTER_DUICONTROL(CTextUI);
		INNER_REGISTER_DUICONTROL(CTreeNodeUI);
		INNER_REGISTER_DUICONTROL(CTreeViewUI);
		INNER_REGISTER_DUICONTROL(CWebBrowserUI);
		INNER_REGISTER_DUICONTROL(CAnimationTabLayoutUI);
		INNER_REGISTER_DUICONTROL(CChildLayoutUI);
		INNER_REGISTER_DUICONTROL(CHorizontalLayoutUI);
		INNER_REGISTER_DUICONTROL(CTabLayoutUI);
		INNER_REGISTER_DUICONTROL(CTileLayoutUI);
		INNER_REGISTER_DUICONTROL(CVerticalLayoutUI);
		INNER_REGISTER_DUICONTROL(CRollTextUI);
		INNER_REGISTER_DUICONTROL(CColorPaletteUI);
		INNER_REGISTER_DUICONTROL(CListExUI);
		INNER_REGISTER_DUICONTROL(CListContainerHeaderItemUI);
		INNER_REGISTER_DUICONTROL(CListTextExtElementUI);
		INNER_REGISTER_DUICONTROL(CHotKeyUI);
		INNER_REGISTER_DUICONTROL(CFadeButtonUI);
		INNER_REGISTER_DUICONTROL(CRingUI);
		INNER_REGISTER_DUICONTROL(CCalendarUI);

		INNER_REGISTER_DUICONTROL(ListView);
		//INNER_REGISTER_DUICONTROL(ImageView);

		INNER_REGISTER_DUICONTROL(WinFrame);
		INNER_REGISTER_DUICONTROL(WinButton);
		INNER_REGISTER_DUICONTROL(WinTabbar);
		INNER_REGISTER_DUICONTROL(WinSplitter);

	}

	CControlFactory::~CControlFactory()
	{
	}

	CControlUI* CControlFactory::CreateControl(CDuiString strClassName)
	{
		strClassName.MakeLower();
		MAP_DUI_CTRATECLASS::iterator iter = m_mapControl.find(strClassName);
		if ( iter == m_mapControl.end()) {
			return NULL;
		}
		else {
			return (CControlUI*) (iter->second());
		}
	}

	void CControlFactory::RegistControl(CDuiString strClassName, CreateClass pFunc)
	{
		strClassName.MakeLower();
		int len = strClassName.GetLength();
		if (len>3 && strClassName[0]==TCHAR('c') && strClassName[len-2]==TCHAR('u') && strClassName[len-1]==TCHAR('i'))
		{
			strClassName.SetAt(len-2, '\0');
			strClassName = strClassName.GetData()+1;
		}
		//m_mapControl.insert(MAP_DUI_CTRATECLASS::value_type(strClassName, pFunc));
		m_mapControl[strClassName] = pFunc;
	}

	CControlFactory* CControlFactory::GetInstance()  
	{
		static CControlFactory* pInstance = new CControlFactory;
		return pInstance;
	}

	void CControlFactory::Release()
	{
		delete this;
	}
}