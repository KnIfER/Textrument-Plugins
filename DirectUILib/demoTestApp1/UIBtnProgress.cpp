#include "StdAfx.h"
#include "UIBtnProgress.h"

#include <atlconv.h>
namespace DuiLib
{
	CBtnProgressUI::CBtnProgressUI() :m_gdiplusToken(0),
		m_dwProgressValue(100),
		m_csBtnText(_T("")),
		m_hAlign(DT_CENTER),
		m_vAlign(DT_CENTER),
		m_EnableEffect(true),
		m_dwDisabledVauleColor(0),
		m_dwDisabledBtnTextColor(0),
		m_iScoreFont(-1),
		m_iBtnTextFont(-1),
		m_EnabledValueStroke(false),
		m_EnabledValueShadow(false),
		m_EnabledBtnTextStroke(false),
		m_EnabledBtnTextShadow(false),
		m_dwStrokeValueColor(0),
		m_dwStrokeBtnTextColor(0),
		m_dwBtnSpacePadding(0)
	{
		m_uValueStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

		m_ShadowValueOffset.X = 0.0f;
		m_ShadowValueOffset.Y = 0.0f;
		m_ShadowValueOffset.Width = 0.0f;
		m_ShadowValueOffset.Height = 0.0f;

		m_ShadowBtnTextOffset.X = 0.0f;
		m_ShadowBtnTextOffset.Y = 0.0f;
		m_ShadowBtnTextOffset.Width = 0.0f;
		m_ShadowBtnTextOffset.Height = 0.0f;

		::ZeroMemory(&m_rcValueTextPadding, sizeof(m_rcValueTextPadding));
		::ZeroMemory(&m_rcBtnTextPadding, sizeof(m_rcBtnTextPadding));
	}
	CBtnProgressUI::~CBtnProgressUI()
	{
		try
		{
			GdiplusShutdown(m_gdiplusToken);
		}
		catch (...)
		{
			throw "CBtnProgressUI::~CBtnProgressUI";
		}
	}
	LPCTSTR CBtnProgressUI::GetClass() const
	{
		return DUI_CTR_BTN_PROGRESS;
	}

	LPVOID CBtnProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_BTN_PROGRESS) == 0) return static_cast<CBtnProgressUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CBtnProgressUI::DoEvent(TEventUI& event)
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
			//判断是否在圆形按钮内点击
			if (IsPointInBtn(event.ptMouse))
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			//判断是否在圆形按钮内点击
			if (IsPointInBtn(event.ptMouse))
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			//return;
		}
		if (event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_BUTTONDOWN)
		{
			if (::PtInRect(&m_rcButton, event.ptMouse))
			{
				//判断是否在圆形按钮内点击
				if(IsPointInBtn(event.ptMouse)&& GetEnabledEffect()) //判断按钮是否处于启用状态
					m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK, event.wParam, event.lParam);

			}
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CControlUI::DoEvent(event);
	}

	void CBtnProgressUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{

		//画图
		RECT rcCtrl,rcDrawCtrl;
		rcCtrl = this->GetPos();
		rcDrawCtrl.left = rcCtrl.left;
		rcDrawCtrl.top = rcCtrl.top;
		rcDrawCtrl.right = rcCtrl.right;
		rcDrawCtrl.bottom = rcCtrl.top + rcCtrl.right - rcCtrl.left;

		//绘制进度条与按钮
		DoDrawProgress(hDC, rcDrawCtrl);
		//绘制进度条数值
		DoDrawScoreValue(hDC);
		//绘制按钮文本
		DoDrawBtnText(hDC);
		CControlUI::DoPaint(hDC, rcPaint, 0); //hhh
	}
	void CBtnProgressUI::SetEnabledEffect(bool _EnabledEffect)
	{
		try
		{
			m_EnableEffect = _EnabledEffect;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetEnabledEffect";
		}
	}


	bool CBtnProgressUI::GetEnabledEffect()
	{
		try
		{
			return m_EnableEffect;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledEffect";
		}
	}

	void CBtnProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		LPTSTR pstr = NULL;
		if (_tcscmp(pstrName, _T("progressColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetProgressColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("outBtnBkColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetOutBtnBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("btnBkColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetBtnBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("btnHotColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetBtnHotColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("btnPushColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetBtnPushColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("progressWidth")) == 0) {
			DWORD dwWidth = _tcstoul(pstrValue, &pstr, 10);
			SetProgressWidth(dwWidth);
		}
		else if (_tcscmp(pstrName, _T("progressRadius")) == 0) {
			DWORD dwRadius = _tcstoul(pstrValue, &pstr, 10);
			SetProgressRadius(dwRadius);
		}
		else if (_tcscmp(pstrName, _T("buttonRadius")) == 0) {
			DWORD dwRadius = _tcstoul(pstrValue, &pstr, 10);
			SetBtnRadius(dwRadius);
		}
		else if (_tcscmp(pstrName, _T("buttonSpace")) == 0) {
			DWORD dwSpace = _tcstoul(pstrValue, &pstr, 10);
			SetBtnSpace(dwSpace);
		}
		else if (_tcscmp(pstrName, _T("scoreValue")) == 0) {
			DWORD dwValue = _tcstoul(pstrValue, &pstr, 10);
			SetScoreValue(dwValue);
		}
		else if (_tcscmp(pstrName, _T("btnText")) == 0) {

			SetBtnTextValue(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("btnTextColor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			DWORD btnTextColor = _tcstoul(pstrValue, &pstr, 16);
			SetBtnTextColor(btnTextColor);
		}
		else if (_tcscmp(pstrName, _T("valueTextpadding")) == 0) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetValueTextPadding(rcTextPadding);
		}
		else if (_tcscmp(pstrName, _T("btnTextpadding")) == 0) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBtnTextPadding(rcTextPadding);
		}
		else if (_tcscmp(pstrName, _T("valueFont")) == 0) SetValueFont(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("btnTextFont")) == 0) SetBtnTextFont(_ttoi(pstrValue));

		else if (_tcscmp(pstrName, _T("btnSpacePadding")) == 0) {
			DWORD dwSpacePadding = _tcstoul(pstrValue, &pstr, 10);
			SetBtnSpacePadding(dwSpacePadding);
		}
		else if (_tcscmp(pstrName, _T("valueAlign")) == 0) {
			if (_tcsstr(pstrValue, _T("left")) != NULL) {
				m_uValueStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				m_uValueStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) {
				m_uValueStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uValueStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL) {
				m_uValueStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_uValueStyle |= DT_RIGHT;
			}
			if (_tcsstr(pstrValue, _T("top")) != NULL) {
				m_uValueStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_VCENTER);
				m_uValueStyle |= (DT_TOP | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
				m_uValueStyle &= ~(DT_TOP | DT_BOTTOM);
				m_uValueStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
				m_uValueStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
				m_uValueStyle |= (DT_BOTTOM | DT_SINGLELINE);
			}
		}
		else if (_tcscmp(pstrName, _T("btnTextAlign")) == 0) {
			if (_tcsstr(pstrValue, _T("left")) != NULL) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				m_uTextStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_uTextStyle |= DT_RIGHT;
			}
			if (_tcsstr(pstrValue, _T("top")) != NULL) {
				m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_VCENTER);
				m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
				m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
				m_uTextStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
				m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
				m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
			}
		}
		else
			CControlUI::SetAttribute(pstrName, pstrValue);
	}
	void CBtnProgressUI::SetProgressColor(DWORD dwColor)
	{
		m_dwProgressBkColor = dwColor;
		m_dwVauleColor = dwColor;
	}
	void CBtnProgressUI::SetOutBtnBkColor(DWORD dwColor)
	{
		m_dwOutBtnBkColor = dwColor;
	}
	void CBtnProgressUI::SetBtnBkColor(DWORD dwColor)
	{
		m_dwBtnBkColor = dwColor;
	}
	void CBtnProgressUI::SetBtnHotColor(DWORD dwColor)
	{
		m_dwBtnHotColor = dwColor;
	}
	void CBtnProgressUI::SetBtnPushColor(DWORD dwColor)
	{
		m_dwBtnPushColor = dwColor;
	}
	void CBtnProgressUI::SetProgressWidth(DWORD dwWidth)
	{
		m_dwProgressWidth = dwWidth;
	}
	void CBtnProgressUI::SetProgressRadius(DWORD dwRadius)
	{
		m_dwProgressRadius = dwRadius;
	}
	void CBtnProgressUI::SetBtnRadius(DWORD dwRadius)
	{
		m_dwBtnRadius = dwRadius;
	}

	void CBtnProgressUI::SetBtnSpace(DWORD dwSpace)
	{
		m_dwBtnSpace = dwSpace;
	}

	void CBtnProgressUI::SetScoreValue(DWORD dwValue)
	{
		if (m_dwProgressValue == dwValue) return;
		m_dwProgressValue = dwValue;
		Invalidate();
	}
	void CBtnProgressUI::SetBtnTextValue(LPCTSTR pstrText)
	{
		if (m_csBtnText == pstrText) return;

		m_csBtnText = pstrText;
		Invalidate();
	}
	void CBtnProgressUI::SetBtnTextColor(DWORD dwValue)
	{
		m_dwBtnTextColor = dwValue;
		Invalidate();
	}
	CDuiString CBtnProgressUI::GetBtnText() const
	{
		return m_csBtnText;
	}

	void CBtnProgressUI::SetValueTextPadding(RECT rc)
	{
		m_rcValueTextPadding = rc;
		Invalidate();
	}
	void CBtnProgressUI::SetBtnTextPadding(RECT rc)
	{
		m_rcBtnTextPadding = rc;
		Invalidate();
	}

	void CBtnProgressUI::SetValueFont(int index)
	{
		m_iScoreFont = index;
		Invalidate();
	}
	void CBtnProgressUI::SetBtnTextFont(int index)
	{
		m_iBtnTextFont = index;
		Invalidate();
	}

	int CBtnProgressUI::GetValueFont() const
	{
		return m_iScoreFont;
	}
	int CBtnProgressUI::GetBtnTextFont() const
	{
		return m_iBtnTextFont;
	}

	void CBtnProgressUI::SetBtnSpacePadding(DWORD dwValue)
	{
		m_dwBtnSpacePadding = dwValue;
	}
	void CBtnProgressUI::EnableBtnStatus(bool bEnable)
	{
		//设置圆形按钮是否被禁用
		SetEnabledEffect(bEnable);

		if (bEnable)
		{
			SetBtnTextColor(m_dwBtnTextColor);
		}
		else
		{
			SetBtnTextColor(m_dwDisabledBtnTextColor);
		}
	}
	bool CBtnProgressUI::GetEnabledValueStroke()
	{
		try
		{
			return m_EnabledValueStroke;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledValueStroke";
		}
	}
	bool CBtnProgressUI::GetEnabledValueShadow()
	{
		try
		{
			return m_EnabledValueShadow;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledValueShadow";
		}
	}
	bool CBtnProgressUI::GetEnabledBtnTextStroke()
	{
		try
		{
			return m_EnabledBtnTextStroke;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledBtnTextStroke";
		}
	}
	bool CBtnProgressUI::GetEnabledBtnTextShadow()
	{
		try
		{
			return m_EnabledBtnTextShadow;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledBtnTextShadow";
		}
	}
	DWORD CBtnProgressUI::GetStrokeValueColor()
	{
		try
		{
			return m_dwStrokeValueColor;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetStrokeValueColor";
		}
	}
	DWORD CBtnProgressUI::GetStrokeBtnTextColor()
	{
		try
		{
			return m_dwStrokeBtnTextColor;
		}
		catch (...)
		{
			throw "CBtnProgressUI::GetEnabledBtnTextStroke";
		}
	}

	void CBtnProgressUI::SetEnabledValueStroke(bool _EnabledStroke)
	{
		try
		{
			m_EnabledValueStroke = _EnabledStroke;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetEnabledValueStroke";
		}
	}
	void CBtnProgressUI::SetEnabledValueShadow(bool _EnabledShadowe)
	{
		try
		{
			m_EnabledValueShadow = _EnabledShadowe;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetEnabledValueShadow";
		}
	}
	void CBtnProgressUI::SetEnabledBtnTextStroke(bool _EnabledStroke)
	{
		try
		{
			m_EnabledBtnTextStroke = _EnabledStroke;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetEnabledBtnTextStroke";
		}
	}
	void CBtnProgressUI::SetEnabledBtnTextShadow(bool _EnabledShadowe)
	{
		try
		{
			m_EnabledBtnTextShadow = _EnabledShadowe;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetEnabledStroke";
		}
	}
	void CBtnProgressUI::SetStrokeValueColor(DWORD _StrokeColor)
	{
		try
		{
			m_dwStrokeBtnTextColor = _StrokeColor;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetStrokeValueColor";
		}
	}
	void CBtnProgressUI::SetStrokeBtnTextColor(DWORD _StrokeColor)
	{
		try
		{
			m_dwStrokeBtnTextColor = _StrokeColor;
		}
		catch (...)
		{
			throw "CBtnProgressUI::SetStrokeBtnTextColor";
		}
	}

	bool CBtnProgressUI::IsPointInBtn(POINT pMouse)
	{
		float fDistance;
		fDistance = ((REAL)pMouse.x - m_pfBtnCenter.X)*((REAL)pMouse.x - m_pfBtnCenter.X) + ((REAL)pMouse.y - m_pfBtnCenter.Y) * ((REAL)pMouse.y - m_pfBtnCenter.Y);
		if (fDistance <= m_dwBtnRadius * m_dwBtnRadius)
			return true;
		return false;
	}

	void CBtnProgressUI::DoDrawProgress(HDC hdc, RECT CtrlRect)
	{
		Graphics graphicCircular(hdc);
		//设置平滑模式
		graphicCircular.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
		//白色画笔
		Pen      outBtnPen(m_dwOutBtnBkColor);
		//按钮轮廓曲线画笔
		Pen      btnBkPen(m_dwBtnBkColor);
		//白色画刷
		SolidBrush  outBtnBrush(m_dwOutBtnBkColor);
		SolidBrush  btnBkBrush(m_dwBtnBkColor);

		//圆形按钮半径参数
		int nInnerBtnRadius, nOutBtnRadius;
		nInnerBtnRadius = m_dwBtnRadius;
		nOutBtnRadius = m_dwBtnRadius + m_dwBtnSpace;

		//进度条圆心坐标
		m_pfCircularCenter.X = CtrlRect.left + ((CtrlRect.right - CtrlRect.left) / 2.0);
		m_pfCircularCenter.Y = CtrlRect.top + ((CtrlRect.bottom - CtrlRect.top) / 2.0);

		//进度条内外半径
		int nInnerRadius;
		int nOutRadius;
		nInnerRadius = (CtrlRect.right - CtrlRect.left) / 2.0 - m_dwProgressWidth;
		nOutRadius = (CtrlRect.right - CtrlRect.left) / 2.0 - m_dwProgressWidth / 2;
		//按钮圆心坐标
		m_pfBtnCenter.X = m_pfCircularCenter.X;
		m_pfBtnCenter.Y = m_pfCircularCenter.Y + nInnerRadius + m_dwProgressWidth / 2.0;

		//记录按钮矩形值
		m_rcButton.left = m_pfBtnCenter.X - m_dwBtnRadius;
		m_rcButton.top = m_pfBtnCenter.Y - m_dwBtnRadius;
		m_rcButton.right = m_pfBtnCenter.X + m_dwBtnRadius;
		m_rcButton.bottom = m_pfBtnCenter.Y + m_dwBtnRadius;

		//绘制进度条
		//画弧线
		Pen      ArcPen(m_dwProgressBkColor, (REAL)m_dwProgressWidth);
		if (m_dwProgressValue > 100)
			m_dwProgressValue = 0;
		//0度对应的初始角度计算
		int nAngleBase = 0;
		//nAngleBase = asin(nOutBtnRadius / (m_pfCircularCenter.Y + nInnerRadius + m_dwProgressWidth / 2.0)) * 180 / PI;
		nAngleBase = asin((nOutBtnRadius - m_dwBtnSpacePadding) / (m_pfBtnCenter.Y - m_pfCircularCenter.Y)) * 180 / PI;
		int nSweepAngle = m_dwProgressValue * (360- 2 * nAngleBase) / 100.0;
		graphicCircular.DrawArc(&ArcPen, m_pfCircularCenter.X - nOutRadius, m_pfCircularCenter.Y - nOutRadius, (REAL)2 * nOutRadius, (REAL)2 * nOutRadius, -1*(270 - nAngleBase), nSweepAngle);

		//绘制按钮
		//画外圆形，白色边线
		graphicCircular.DrawEllipse(&outBtnPen, m_pfBtnCenter.X - nOutBtnRadius, m_pfBtnCenter.Y - nOutBtnRadius, (REAL)2 * nOutBtnRadius, (REAL)2 * nOutBtnRadius);
		graphicCircular.FillEllipse(&outBtnBrush, m_pfBtnCenter.X - nOutBtnRadius, m_pfBtnCenter.Y - nOutBtnRadius, (REAL)2 * nOutBtnRadius, (REAL)2 * nOutBtnRadius);
		//画内圆
		graphicCircular.DrawEllipse(&btnBkPen, m_pfBtnCenter.X - nInnerBtnRadius, m_pfBtnCenter.Y - nInnerBtnRadius, (REAL)2 * nInnerBtnRadius, (REAL)2 * nInnerBtnRadius);
		graphicCircular.FillEllipse(&btnBkBrush, m_pfBtnCenter.X - nInnerBtnRadius, m_pfBtnCenter.Y - nInnerBtnRadius, (REAL)2 * nInnerBtnRadius, (REAL)2 * nInnerBtnRadius);
	}

	void CBtnProgressUI::DoDrawScoreValue(HDC hdc)
	{
		if (m_dwVauleColor == 0) m_dwVauleColor = m_pManager->GetDefaultFontColor();
		if (m_dwDisabledVauleColor == 0) m_dwDisabledVauleColor = m_pManager->GetDefaultDisabledColor();

		RECT rc;
		rc.left = m_pfCircularCenter.X - m_dwProgressRadius;
		rc.top = m_pfCircularCenter.Y - m_dwProgressRadius;
		rc.right = m_pfCircularCenter.X + m_dwProgressRadius;
		rc.bottom = m_pfCircularCenter.Y + m_dwProgressRadius;

		rc.left += m_rcValueTextPadding.left;
		rc.right -= m_rcValueTextPadding.right;
		rc.top += m_rcValueTextPadding.top;
		rc.bottom -= m_rcValueTextPadding.bottom;


		//if (!GetEnabledEffect())
		//{
		//	if (IsEnabled()) {
		//		CRenderEngine::DrawText(hdc, m_pManager, rc, m_sText, m_dwVauleColor, \
				//			m_iScoreFont, DT_SINGLELINE | m_uTextStyle);
//	}
//	else
//	{
//		CRenderEngine::DrawText(hdc, m_pManager, rc, m_sText, m_dwDisabledVauleColor, \
		//			m_iScoreFont, DT_SINGLELINE | m_uTextStyle);
//	}
//}
//else
		{
			Gdiplus::Font	nFont(hdc, m_pManager->GetFont(GetValueFont()));
			Graphics nGraphics(hdc);
			//nGraphics.SetTextRenderingHint(m_TextRenderingHintAntiAlias);
			//Pen pen(m_dwVauleColor);
			//nGraphics.DrawRectangle(&pen, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			StringFormat format;
			format.SetAlignment((StringAlignment)m_hAlign);
			format.SetLineAlignment((StringAlignment)m_vAlign);

			RectF nRc((float)rc.left, (float)rc.top, (float)rc.right - rc.left, (float)rc.bottom - rc.top);
			RectF nShadowRc = nRc;
			nShadowRc.X += m_ShadowValueOffset.X;
			nShadowRc.Y += m_ShadowValueOffset.Y;

			int nGradientLength = 0;

			if (nGradientLength == 0)
				nGradientLength = (rc.bottom - rc.top);

			//LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0), Point(0, nGradientLength), _MakeRGB(GetTransShadow(), GetTextShadowColorA()), _MakeRGB(GetTransShadow1(), GetTextShadowColorB() == -1 ? GetTextShadowColorA() : GetTextShadowColorB()));
			//LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0), Point(0, nGradientLength), _MakeRGB(GetTransText(), GetTextColor()), _MakeRGB(GetTransText1(), GetTextColor1() == -1 ? GetTextColor() : GetTextColor1()));
			SolidBrush valueBrush(m_dwVauleColor);
			CDuiString csText;
			csText.Format(_T("%d"), m_dwProgressValue);

			if (GetEnabledValueStroke() && GetStrokeValueColor() > 0)
			{
#ifdef _UNICODE
				nRc.Offset(-1, 0);
				nGraphics.DrawString(csText, csText.GetLength(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(2, 0);
				nGraphics.DrawString(csText, csText.GetLength(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(csText, csText.GetLength(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(0, 2);
				nGraphics.DrawString(csText, csText.GetLength(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(0, -1);
#else
				USES_CONVERSION;
				std::wstring mTextValue = A2W(csText.GetData());

				nRc.Offset(-1, 0);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(2, 0);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(0, 2);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &valueBrush);
				nRc.Offset(0, -1);
#endif
			}
#ifdef _UNICODE


			nGraphics.DrawString(csText, csText.GetLength(), &nFont, nRc, &format, &valueBrush);
#else
			USES_CONVERSION;
			wstring mTextValue = A2W(csText.GetData());

			nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &valueBrush);
#endif

		}
	}
	void CBtnProgressUI::DoDrawBtnText(HDC hdc)
	{
		if (m_dwBtnTextColor == 0) m_dwBtnTextColor = m_pManager->GetDefaultFontColor();
		if (m_dwDisabledBtnTextColor == 0) m_dwDisabledBtnTextColor = m_pManager->GetDefaultDisabledColor();

		RECT rc;
		rc.left = m_pfBtnCenter.X - m_dwBtnRadius;
		rc.top = m_pfBtnCenter.Y - m_dwBtnRadius;
		rc.right = m_pfBtnCenter.X + m_dwBtnRadius;
		rc.bottom = m_pfBtnCenter.Y + m_dwBtnRadius;

		rc.left += m_rcBtnTextPadding.left;
		rc.right -= m_rcBtnTextPadding.right;
		rc.top += m_rcBtnTextPadding.top;
		rc.bottom -= m_rcBtnTextPadding.bottom;

		{
			Gdiplus::Font	nFont(hdc, m_pManager->GetFont(GetBtnTextFont()));
			Graphics nGraphics(hdc);
			//nGraphics.SetTextRenderingHint(m_TextRenderingHintAntiAlias);
			//Pen pen(m_dwBtnTextColor);
			//nGraphics.DrawRectangle(&pen, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			StringFormat format;
			format.SetAlignment((StringAlignment)m_hAlign);
			format.SetLineAlignment((StringAlignment)m_vAlign);

			RectF nRc((float)rc.left, (float)rc.top, (float)rc.right - rc.left, (float)rc.bottom - rc.top);
			RectF nShadowRc = nRc;
			nShadowRc.X += m_ShadowBtnTextOffset.X;
			nShadowRc.Y += m_ShadowBtnTextOffset.Y;

			int nGradientLength = 0;

			if (nGradientLength == 0)
				nGradientLength = (rc.bottom - rc.top);

			//LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0), Point(0, nGradientLength), _MakeRGB(GetTransShadow(), GetTextShadowColorA()), _MakeRGB(GetTransShadow1(), GetTextShadowColorB() == -1 ? GetTextShadowColorA() : GetTextShadowColorB()));
			//LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0), Point(0, nGradientLength), _MakeRGB(GetTransText(), GetTextColor()), _MakeRGB(GetTransText1(), GetTextColor1() == -1 ? GetTextColor() : GetTextColor1()));
			SolidBrush btnTextBrush(m_dwBtnTextColor);

			if (GetEnabledValueStroke() && GetStrokeValueColor() > 0)
			{
#ifdef _UNICODE
				nRc.Offset(-1, 0);
				nGraphics.DrawString(m_csBtnText, m_csBtnText.GetLength(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(2, 0);
				nGraphics.DrawString(m_csBtnText, m_csBtnText.GetLength(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(m_csBtnText, m_csBtnText.GetLength(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(0, 2);
				nGraphics.DrawString(m_csBtnText, m_csBtnText.GetLength(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(0, -1);
#else
				USES_CONVERSION;
				wstring mTextValue = A2W(m_csBtnText.GetData());

				nRc.Offset(-1, 0);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(2, 0);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(0, 2);
				nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &btnTextBrush);
				nRc.Offset(0, -1);
#endif
			}
#ifdef _UNICODE


			nGraphics.DrawString(m_csBtnText, m_csBtnText.GetLength(), &nFont, nRc, &format, &btnTextBrush);
#else
			USES_CONVERSION;
			wstring mTextValue = A2W(m_csBtnText.GetData());

			nGraphics.DrawString(mTextValue.c_str(), mTextValue.length(), &nFont, nRc, &format, &btnTextBrush);
#endif

		}
	}
}