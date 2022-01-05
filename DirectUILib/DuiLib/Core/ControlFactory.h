#pragma once
namespace DuiLib 
{
	typedef CControlUI* (*CreateClass)();

	class UILIB_API CControlFactory
	{
	public:
		CControlUI* CreateControl(const QkString & strClassName);
		void RegistControl(QkString & strClassName, CreateClass pFunc);
		void RegistControl(LPCTSTR strClassName, CreateClass pFunc);

		static CControlFactory* GetInstance();
		void Release();

	private:	
		CControlFactory();
		virtual ~CControlFactory();

	private:
		QkStringPtrMap m_mapControl;
	};

#define DECLARE_QKCONTROL(class_name)\
public:\
	static CControlUI* CreateControl();

#define IMPLEMENT_QKCONTROL(class_name)\
	CControlUI* class_name::CreateControl()\
	{\
		return new class_name;\
	}

#define REGIST_QKCONTROL(class_name)\
	CControlFactory::GetInstance()->RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);

}