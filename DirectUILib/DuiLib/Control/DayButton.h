#pragma once

namespace DuiLib
{
	class UILIB_API DayButtonUI : public CButtonUI
	{
		DECLARE_DUICONTROL(DayButtonUI)

	public:
		DayButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetCalendarValDest(LPCTSTR pstrValue);
		LPCTSTR GetCalendarValDest();
		void SetCalendarName(LPCTSTR pStrCalendarName);
		LPCTSTR GetCalendarName();
		void SetCalendarStyle(LPCTSTR pStrCalendarStyle);
		LPCTSTR GetCalendarStyle();
		void SetCalendarProfile(LPCTSTR pStrCalendarProfile);
		LPCTSTR GetCalendarProfile();


		SIZE EstimateSize(SIZE szAvailable);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		unUserData GetAttribute( LPCTSTR pstrName );



	protected:

		CDuiString	m_sSalendarValDest;
		CDuiString	m_sCalendarName;
		CDuiString	m_sCalendarStyle;
		CDuiString	m_sCalendarProfile;

	};

}	// namespace DuiLib