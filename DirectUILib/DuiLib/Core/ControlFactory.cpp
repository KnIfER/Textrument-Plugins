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
		FACTORY_REGISTER_QKCONTROL(Button);
		FACTORY_REGISTER_QKCONTROL(CComboUI);
		FACTORY_REGISTER_QKCONTROL(CComboBoxUI);
		FACTORY_REGISTER_QKCONTROL(CDateTimeUI);
		//FACTORY_REGISTER_QKCONTROL(CEditUI);
		FACTORY_REGISTER_ALIAS(WinEdit, CEditUI);
		FACTORY_REGISTER_QKCONTROL(InputBox);
		FACTORY_REGISTER_ALIAS(Edit, InputBox);
		FACTORY_REGISTER_ALIAS(Input, InputBox);
		FACTORY_REGISTER_QKCONTROL(CActiveXUI);
		FACTORY_REGISTER_QKCONTROL(CGifAnimUI);
		FACTORY_REGISTER_QKCONTROL(CLabelUI);
		FACTORY_REGISTER_QKCONTROL(CListUI);
		FACTORY_REGISTER_QKCONTROL(CListHeaderUI);
		FACTORY_REGISTER_QKCONTROL(CListHeaderItemUI);
		FACTORY_REGISTER_QKCONTROL(CListLabelElementUI);
		FACTORY_REGISTER_QKCONTROL(CListTextElementUI);
		FACTORY_REGISTER_QKCONTROL(CListContainerElementUI);
		FACTORY_REGISTER_QKCONTROL(CMenuUI);
		FACTORY_REGISTER_QKCONTROL(CMenuElementUI);
		FACTORY_REGISTER_ALIAS(Option, OptionBtn);
		FACTORY_REGISTER_QKCONTROL(CCheckBoxUI);
		FACTORY_REGISTER_QKCONTROL(CProgressUI);
		FACTORY_REGISTER_QKCONTROL(CRichEditUI);
		FACTORY_REGISTER_QKCONTROL(CScrollBarUI);
		FACTORY_REGISTER_QKCONTROL(CSliderUI);
		FACTORY_REGISTER_QKCONTROL(CTextUI);
		FACTORY_REGISTER_QKCONTROL(CTreeNodeUI);
		FACTORY_REGISTER_QKCONTROL(CTreeViewUI);
		FACTORY_REGISTER_QKCONTROL(CWebBrowserUI);
		FACTORY_REGISTER_QKCONTROL(CChildLayoutUI);
		FACTORY_REGISTER_QKCONTROL(CHorizontalLayoutUI);
		FACTORY_REGISTER_ALIAS(HBox, CHorizontalLayoutUI);
		FACTORY_REGISTER_ALIAS(HorBox, CHorizontalLayoutUI);
		FACTORY_REGISTER_QKCONTROL(CTabLayoutUI);
		FACTORY_REGISTER_ALIAS(AnimationTabLayout, CTabLayoutUI);
		FACTORY_REGISTER_QKCONTROL(CTileLayoutUI);
		FACTORY_REGISTER_QKCONTROL(CVerticalLayoutUI);
		FACTORY_REGISTER_ALIAS(VBox, CVerticalLayoutUI);
		FACTORY_REGISTER_ALIAS(VertBox, CVerticalLayoutUI);
		FACTORY_REGISTER_QKCONTROL(CCalendarUI);

		FACTORY_REGISTER_QKCONTROL(ListView);
		//FACTORY_REGISTER_QKCONTROL(ImageView);

		FACTORY_REGISTER_QKCONTROL(WinFrame);
		FACTORY_REGISTER_QKCONTROL(WinButton);
		FACTORY_REGISTER_QKCONTROL(WinTabbar);
		FACTORY_REGISTER_QKCONTROL(WinSplitter);

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