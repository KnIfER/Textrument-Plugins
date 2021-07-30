#include "UIBeautifulSwitch.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(CBSwitchUI)
	
	void CBSwitchUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			//::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			//return;
		}
		if (event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_BUTTONDOWN)
		{
			Selected(!m_bSelected, true);
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CControlUI::DoEvent(event);
	}

	LPCTSTR CBSwitchUI::GetClass() const
	{
		return _T("BSwitch");
	}

	void CBSwitchUI::PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics graph(hDC);
		graph.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);// SmoothingMode

		RECT rcPos = GetPos();

		// 背景色， 黄色
		Gdiplus::SolidBrush bkbrush(m_dwBackColor);
		//graph.FillPie(&bkbrush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);

		//graph.FillEllipse(&bkbrush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);


		auto top = rcPos.top;
		auto left = rcPos.left;

		auto w = rcPos.right - left;
		auto h = rcPos.bottom - top;


		//graph.FillRectangle(&bkbrush, left, top, w, h);


		long circleW = w/2;

		long rectW = circleW * 0.8f;

		long circleH = h;

		graph.FillPie(&bkbrush, left, top, circleW, circleH, 90-2, 180+4);

		graph.FillRectangle(&bkbrush, left+circleW/2+-1, top, rectW, circleH);


		graph.FillPie(&bkbrush, left+rectW+-2, top, circleW, circleH, 270-2, 180+4);


		bkbrush.SetColor(0xFFFFFFFF);


		if(m_bSelected) 
		{
			int pad=2;
			graph.FillPie(&bkbrush, left+rectW+-pad, top+pad, circleH-pad, circleH-pad*2, 0, 360);
		}
		else
		{
			int pad=2;
			graph.FillPie(&bkbrush, left+pad, top+pad, circleH-pad, circleH-pad*2, 0, 360);
		}


		// 前景色， 灰色
		//Gdiplus::SolidBrush forebrush(m_dwForeColor);
		//int nStartDegree = 90;
		//int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		//graph.FillPie(&forebrush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);


		// 前景色， 白色
		//Gdiplus::SolidBrush coverbrush(0xFFFFFFFF);
		//CDuiRect rcCenter = rcPos;
		//rcCenter.left +=10;
		//rcCenter.top += 10;
		//rcCenter.right -= 10;
		//rcCenter.bottom -= 10;
		//graph.FillPie(&coverbrush, rcCenter.left, rcCenter.top, rcCenter.right - rcCenter.left, rcCenter.bottom - rcCenter.top, 0, 360);

		graph.ReleaseHDC(hDC);
	}

	void CBSwitchUI::PaintForeColor(HDC hDC)
	{
	}

	void CBSwitchUI::PaintForeImage(HDC hDC)
	{

	}
} // namespace DuiLib
