#pragma once

namespace DuiLib
{
	class UILIB_API TestButton : public Button
	{
		DECLARE_QKCONTROL(TestButton)

	public:
		TestButton();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;





	protected:

		QkString	m_sSalendarValDest;
		QkString	m_sCalendarName;
		QkString	m_sCalendarStyle;
		QkString	m_sCalendarProfile;

	};

}	// namespace DuiLib