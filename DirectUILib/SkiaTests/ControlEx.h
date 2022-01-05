#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include "StdAfx.h"
#include "UIBtnProgress.h"

class CCircleProgressUI : public CProgressUI
{
	DECLARE_QKCONTROL(CCircleProgressUI)
public:
	CCircleProgressUI()
	{
	}

	void DoEvent(TEventUI& event)
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
			//if (::PtInRect(&m_rcButton, event.ptMouse))
			//{
			//	//判断是否在圆形按钮内点击
			//	if(IsPointInBtn(event.ptMouse)&& GetEnabledEffect()) //判断按钮是否处于启用状态
			isChecked=!isChecked;
					_manager->SendNotify(this, DUI_MSGTYPE_CLICK, event.wParam, event.lParam);

					Invalidate();

			//}
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CControlUI::DoEvent(event);
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgress" );
	}

	void PaintBkColor(HDC hDC)
	{

	}

	void PaintForeColor(HDC hDC)
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


		if(isChecked) 
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

	void PaintForeImage(HDC hDC)
	{
		
	}

private:
	bool isChecked;
};


class CMyComboUI : public CComboUI
{
	DECLARE_QKCONTROL(CMyComboUI)
public:
	CMyComboUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("MyCombo" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}
};

#define CHARTVIEW_PIE		0x0
#define CHARTVIEW_HISTOGRAM 0x01

typedef struct tagCHARTITEM
{
	QkString name;
	double value;
} CHARTITEM, *PCHARTITEM;

class CChartViewUI : 
	public CControlUI
{
public:
	DECLARE_QKCONTROL(CChartViewUI)
public:
	CChartViewUI(void);
	~CChartViewUI(void);

	bool Add(LPCTSTR name, double value);
	bool AddAt(LPCTSTR name, double value, int iIndex);

public:
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoPaintPie(HDC hDC, const RECT& rcPaint);
	virtual void DoPaintHistogram(HDC hDC, const RECT& rcPaint);
private:
	vector<CHARTITEM> m_items;
	int m_ViewStyle;
	QkString m_sShadowImage;
	int m_ShadowImageHeight;
	QkString m_sPillarImage;
	int m_PillarImageWidth;
	DWORD m_dwTextColor;
	DWORD m_dwDisabledTextColor;
	bool m_bShowHtml;
	bool m_bShowText;
	int m_iFont;
};

class CCircleProgressExUI : public CProgressUI
{
	DECLARE_QKCONTROL(CCircleProgressExUI)
public:
	CCircleProgressExUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgressEx");
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};

class CWndUI : public CControlUI
{
	DECLARE_QKCONTROL(CWndUI)
public:
	CWndUI()
	{

	}

	LPCTSTR GetClass() const
	{
		return _T("Wnd");
	}
public:
	void Attach(HWND hWnd)
	{
		m_hWnd = hWnd;
		AdjustPos();
	}

	HWND Detach()
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		return hWnd;
	}

	virtual void SetVisible(bool bVisible = true)
	{
		CControlUI::SetVisible(bVisible);
		AdjustPos();
	}

	virtual void SetInternVisible(bool bVisible = true)
	{
		CControlUI::SetInternVisible(bVisible);
		AdjustPos();
	}

	virtual void SetPos(RECT rc, bool bNeedInvalidate /* = true */)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		AdjustPos();
	}

	void AdjustPos()
	{
		if(::IsWindow(m_hWnd)) {
			if(_manager) {
				RECT rcItem = m_rcItem;
				if(!::IsChild(_manager->GetPaintWindow(), m_hWnd)) {
					RECT rcWnd = {0};
					::GetWindowRect(_manager->GetPaintWindow(), &rcWnd);
					::OffsetRect(&rcItem, rcWnd.left, rcWnd.top);
				}
				SetWindowPos(m_hWnd, NULL, rcItem.left, rcItem.top, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top, SWP_NOACTIVATE | SWP_NOZORDER);
			}
			ShowWindow(m_hWnd, IsVisible() ? SW_SHOW : SW_HIDE);
		}
	}

protected:
	HWND m_hWnd;
};
#endif __CONTROLEX_H__
