#ifdef QkUICalendar

#ifndef Calendar_h__
#define Calendar_h__

#include <map>

#pragma once

namespace DuiLib
{
	typedef struct tag_CalendarStyle
	{
		QkString	nCalenderBorderColor;		//日历边框颜色
		QkString	nCalendarBkColor;			//日历背景颜色
		QkString	nMainTitleBkColor;			//日历主标题背景颜色
		QkString	nSubTitleBkColor;			//日历副标题背景颜色
		QkString	nWeekendColorA;				//周末的日期隔行背景颜色
		QkString	nWeekendColorB;				//周末的日期隔行背景颜色
		QkString	nDayHotColor;				//日期获得焦点时背景颜色
		QkString	nDayPushedColor;			//日期被按下时背景颜色
		QkString	nDaySelectColor;			//日期被选中时背景颜色
		QkString	nDayDisabledColor;			//日期被禁用时的背景色
		QkString	nNoCurMonthDayColor;		//非本月日期的背景颜色
		QkString	nWeekIntervalColorA;		//周隔行颜色A
		QkString	nWeekIntervalColorB;		//周隔行颜色B
		QkString	nStatusBkColor;				//底部信息栏背景色
	}TCalendarStyle;
	
	typedef struct tag_SubTitleString
	{
		QkString	nSundayStr;
		QkString	nMondayStr;
		QkString	nTuesdayStr;
		QkString	nWednesdayStr;
		QkString	nThursdayStr;
		QkString	nFridayStr;
		QkString	nSaturdayStr;
		QkString	nToDayString;
	}TSubTitleString;

	typedef struct tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;
		int		nWeek;
		int		nWeekLine;
	}TCalendarInfo;

	class ICalendar
	{
	public:
		virtual CControlUI* CreateWeekPanel(CControlUI* _Panent,int _Week) {return NULL;};
		virtual CControlUI* CreateDayPanel(int _Week,LPCTSTR _GroupName) {return NULL;};
		virtual CControlUI* CreateInfoPanel(){return NULL;};
		virtual void InitWeekPanel(CControlUI* _Control,int _Week) {};
		virtual	void InitDayPanel(CControlUI* _Control,TCalendarInfo& _CalendarInfo) {};
		virtual void InitDayPanel(CControlUI* _Control,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth) {};
		virtual void OnCalendarUpdateStart(int _Year,int _Mooth) {};
		virtual void OnCalendarUpdateEnd(int _Year,int _Mooth) {};
		virtual void OnCalendarChange(int _Year,int _Mooth,int _Day){};
	};
	
	class CCalendarDlg;

	class CCalendarUI : public CVerticalLayoutUI,public ICalendar
	{
		friend class CCalendarDlg;

		DECLARE_QKCONTROL(CCalendarUI)

	public:
		CCalendarUI(void);
		~CCalendarUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoEvent(TEventUI& event);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CControlUI* CreateWeekPanel(CControlUI* _Panent,int _Week);
		CControlUI* CreateDayPanel(int _Week,LPCTSTR _GroupName);
		CControlUI* CreateInfoPanel();
		void InitWeekPanel(CControlUI* _Control,int _Week);
		void InitDayPanel(CControlUI* _Control,TCalendarInfo& _CalendarInfo);
		void InitDayPanel(CControlUI* _Control,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth);
		void OnCalendarUpdateStart(int _Year,int _Mooth);
		void OnCalendarChange(int _Year,int _Mooth,int _Day);
	public:
		bool IsLeapYear(int _Year);
		int  GetNumLeapYear(int _Year);
		int  DaysOfMonth(int _Mooth,int _Year = -1);
		void InitCalendarDis(int _Year,int _Month);
		void CalDateTime(TCalendarInfo* _TCalendarInfo);
		void CalDateTime(int _Year,int _Mooth,int _Day);
		QkString	GetCurSelDateTime();
		void		SetComboTargetName(LPCTSTR pstrName);
		QkString	GetComTargetName();
		CControlUI*	GetComTargetObj();
	public:
		void SetCallback(ICalendar* _ICalendar);

	public:
		bool SetSubTitleString(LPCTSTR _Name);
		void SetSubTitleString(TSubTitleString& _SubTitleString);
		void SetCalendarStyle(TCalendarStyle& _TCalendarStyle);
		bool AddSubTitleString(LPCTSTR _Name,TSubTitleString& _SubTitleString);
		bool AddSubTitleString(LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday);
		TSubTitleString& GetSubTitleString(LPCTSTR _Name = NULL);
		bool RemoveAtSubTitleString(LPCTSTR _Name);
		void RemoveAllSubTitleString();

	public:
		bool OnLastYear(void* _Param);
		bool OnMoothSelect(void* _Param);
		bool OnNextYear(void* _Param);
		bool OnSelectMooth(void* _Param);
		bool OnSelcetDay(void* _Param);
		bool OnToday(void* _Param);

	public:
		void SetEnabledMoothSel(bool _Enabled = true);
		bool GetEnabledMoothSel();
		void SetEnabledYearSel(bool _Enabled = true);
		bool GetEnabledYearSel();
		void SetMainTitleHeight(int _Height);
		int GetMainTitleHeight();
		void SetSubTitleHeight(int _Height);
		int GetSubTitleHeight();
		void SetStatusInfoHeight(int _Height);
		int GetStatusInfoHeight();

		void AnyCalendarStyle(LPCTSTR _StyleStr,TCalendarStyle* _TCalendarStyle = NULL);

		static void ShowCalendarDlg(CControlUI* _pControl);
	private:
		QkString				m_sComboTargetName;
		QkString				mDateTime;
		CHorizontalLayoutUI*	pMainTitleHoriz;
		Button*				pLastYearBtn;
		Button*				pMoothSelectBtn;
		Button*				pNextYearBtn;

		CHorizontalLayoutUI*	pSubTitleHoriz;
		Button*				pSundayBtn;
		Button*				pMondayBtn;
		Button*				pTuesdayBtn;
		Button*				pWednesdayBtn;
		Button*				pThursdayBtn;
		Button*				pFridayBtn;
		Button*				pSaturdayBtn;

		Button*				pDateTimeBtn;
		Button*				pToDayBtn;

		CHorizontalLayoutUI*	pMoothPanelHorz;
		CVerticalLayoutUI*		pWeekPanelVert;
		CHorizontalLayoutUI*	pInfoPanelHorz;

		QkString				mLastYearBtnString;
		QkString				mMoothSelectBtnStrign;
		QkString				mNextYearBtnString;

		int						mToday;
		bool					pEnabledYearSel;
		bool					pEnabledMoothSel;
		struct tm*				pCurDateTime;

		ICalendar*				pICalendar;

		TCalendarStyle			m_DefaultStyle;
		TSubTitleString			mSubTitleString;
		CStdPtrArray			mTCalendarInfoArray;
		QkStringPtrMap		mSubTitleStringArray;
	};
}

#endif // Calendar_h__


#endif