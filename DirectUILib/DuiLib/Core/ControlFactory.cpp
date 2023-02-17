#include "StdAfx.h"
#include "ControlFactory.h"

namespace DuiLib 
{
	CControlFactory::CControlFactory()
	{
		QkString factoryBuffer;
		factoryBuffer.AsBuffer();
		#define FACTORY_REGISTER_QKCONTROL(class_name)\
			factoryBuffer = _T(#class_name);\
			RegistControl(factoryBuffer, (CreateClass)class_name::CreateControl);
		#define FACTORY_REGISTER_ALIAS(name, class_name)\
			factoryBuffer = _T(#name);\
			RegistControl(factoryBuffer, (CreateClass)class_name::CreateControl);

		FACTORY_REGISTER_QKCONTROL(CControlUI);
		FACTORY_REGISTER_QKCONTROL(CContainerUI);
		FACTORY_REGISTER_ALIAS(PAD, CControlUI);
		FACTORY_REGISTER_ALIAS(BOX, CContainerUI);

#ifdef QkUIChildLayout:
		FACTORY_REGISTER_QKCONTROL(CChildLayoutUI);
#endif
#ifdef QkUIHorizontalLayout:
		FACTORY_REGISTER_QKCONTROL(CHorizontalLayoutUI);
		FACTORY_REGISTER_ALIAS(HBox, CHorizontalLayoutUI);
		FACTORY_REGISTER_ALIAS(HorBox, CHorizontalLayoutUI);
#endif
#ifdef QkUIVerticalLayout:
		FACTORY_REGISTER_QKCONTROL(CVerticalLayoutUI);
		FACTORY_REGISTER_ALIAS(VBox, CVerticalLayoutUI);
		FACTORY_REGISTER_ALIAS(VertBox, CVerticalLayoutUI);
#endif
#ifdef QkWinFrame:
		FACTORY_REGISTER_QKCONTROL(WinFrame);
#endif
#ifdef QkUIButton:
		FACTORY_REGISTER_QKCONTROL(Button);
#endif
#ifdef QkUICombo:
		FACTORY_REGISTER_QKCONTROL(CComboUI);
#endif
#ifdef QkUIComboBox:
		FACTORY_REGISTER_QKCONTROL(CComboBoxUI);
#endif
#ifdef QkUIDateTime:
		FACTORY_REGISTER_QKCONTROL(CDateTimeUI);
#endif
#ifdef QkUIEdit:
		//FACTORY_REGISTER_QKCONTROL(CEditUI);
		FACTORY_REGISTER_ALIAS(WinEdit, CEditUI);
#endif
#ifdef QkInputBox
		FACTORY_REGISTER_QKCONTROL(InputBox);
		FACTORY_REGISTER_ALIAS(Edit, InputBox);
		FACTORY_REGISTER_ALIAS(Input, InputBox);
#endif
#ifdef QkUIActiveX:
		FACTORY_REGISTER_QKCONTROL(CActiveXUI);
#endif
#ifdef QkUIGifAnim:
		FACTORY_REGISTER_QKCONTROL(CGifAnimUI);
#endif
#ifdef QkUILabel:
		FACTORY_REGISTER_QKCONTROL(CLabelUI);
#endif
#ifdef QkUIList:
		FACTORY_REGISTER_QKCONTROL(CListUI);
#endif
#ifdef QkUIListHeader:
		FACTORY_REGISTER_QKCONTROL(CListHeaderUI);
#endif
#ifdef QkUIListHeaderItem:
		FACTORY_REGISTER_QKCONTROL(CListHeaderItemUI);
#endif
#ifdef QkUIListLabelElement:
		FACTORY_REGISTER_QKCONTROL(CListLabelElementUI);
#endif
#ifdef QkUIListTextElement:
		FACTORY_REGISTER_QKCONTROL(CListTextElementUI);
#endif
#ifdef QkUIListContainerElement:
		FACTORY_REGISTER_QKCONTROL(CListContainerElementUI);
#endif
#ifdef QkUIMenu:
		FACTORY_REGISTER_QKCONTROL(CMenuUI);
#endif
#ifdef QkUIMenuElement:
		FACTORY_REGISTER_QKCONTROL(CMenuElementUI);
#endif
#ifdef QkUIOption
		FACTORY_REGISTER_ALIAS(Option, OptionBtn);
#endif
#ifdef QkUICheckBox:
		FACTORY_REGISTER_QKCONTROL(CCheckBoxUI);
#endif
#ifdef QkUIProgress:
		FACTORY_REGISTER_QKCONTROL(CProgressUI);
#endif
#ifdef QkUIRichEdit:
		FACTORY_REGISTER_QKCONTROL(CRichEditUI);
#endif
#ifdef QkUIScrollBar:
		FACTORY_REGISTER_QKCONTROL(CScrollBarUI);
#endif
#ifdef QkUISlider:
		FACTORY_REGISTER_QKCONTROL(CSliderUI);
#endif
#ifdef QkUIText:
		FACTORY_REGISTER_QKCONTROL(CTextUI);
#endif
#ifdef QkUITreeNode:
		FACTORY_REGISTER_QKCONTROL(CTreeNodeUI);
#endif
#ifdef QkUITreeView:
		FACTORY_REGISTER_QKCONTROL(CTreeViewUI);
#endif
#ifdef QkUIWebBrowser:
		FACTORY_REGISTER_QKCONTROL(CWebBrowserUI);
#endif
#ifdef QkUITabLayout:
		FACTORY_REGISTER_QKCONTROL(CTabLayoutUI);
		FACTORY_REGISTER_ALIAS(AnimationTabLayout, CTabLayoutUI);
#endif
#ifdef QkUITileLayout:
		FACTORY_REGISTER_QKCONTROL(CTileLayoutUI);
#endif
#ifdef QkUICalendar:
		FACTORY_REGISTER_QKCONTROL(CCalendarUI);
#endif

#ifdef QkListView
		FACTORY_REGISTER_QKCONTROL(ListView);
#endif
		//FACTORY_REGISTER_QKCONTROL(ImageView);
#ifdef QkWinButton
		FACTORY_REGISTER_QKCONTROL(WinButton);
#endif
#ifdef QkWinTabbar
		FACTORY_REGISTER_QKCONTROL(WinTabbar);
#endif
#ifdef QkWinSplitter
		FACTORY_REGISTER_QKCONTROL(WinSplitter);
#endif

	}

	CControlFactory::~CControlFactory()
	{
	}

	CControlUI* CControlFactory::CreateControl(const QkString & strClassName)
	{
		CreateClass pFunc = static_cast<CreateClass>(m_mapControl.Find(strClassName));
		return pFunc?(CControlUI*) (pFunc()):NULL;
	}

	void CControlFactory::RegistControl(QkString & strClassName, CreateClass pFunc)
	{
		strClassName.MakeLower();
		int len = strClassName.GetLength();
		if (len>3 && strClassName[0]==TCHAR('c') && strClassName[len-2]==TCHAR('u') && strClassName[len-1]==TCHAR('i'))
		{
			strClassName.MidFast(1, len-3);
		}
		m_mapControl.Set(strClassName, pFunc);
	}

	void CControlFactory::RegistControl(LPCTSTR strClassName, CreateClass pFunc)
	{
		QkString buffer = strClassName;
		RegistControl(buffer, pFunc);
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