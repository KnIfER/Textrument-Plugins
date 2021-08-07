#pragma once

namespace DuiLib
{
	class UILIB_API TestButton : public CButtonUI
	{
		DECLARE_DUICONTROL(TestButton)

	public:
		TestButton();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;





	protected:

		CDuiString	m_sSalendarValDest;
		CDuiString	m_sCalendarName;
		CDuiString	m_sCalendarStyle;
		CDuiString	m_sCalendarProfile;

	};

}	// namespace DuiLib