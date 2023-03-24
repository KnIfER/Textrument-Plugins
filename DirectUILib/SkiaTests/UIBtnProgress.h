#ifndef UI_BTN_PROGRESS_H
#define UI_BTN_PROGRESS_H
#pragma once

#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;
class  Gdiplus::RectF;
struct  Gdiplus::GdiplusStartupInput;

#define PI  3.1415926

#define  DUI_CTR_BTN_PROGRESS					 (_T("btnProgress"))

namespace DuiLib
{
	class  CBtnProgressUI : public CControlUI
	{
	public:
		CBtnProgressUI();
		~CBtnProgressUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void		SetEnabledEffect(bool _EnabledEffect);
		bool		GetEnabledEffect();
		void DoEvent(TEventUI& event);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		//设置进度条颜色
		void SetProgressColor(DWORD dwColor);
		//设置按钮外部圆环的颜色
		void SetOutBtnBkColor(DWORD dwColor);
		//设置按钮正常颜色
		void SetBtnBkColor(DWORD dwColor);
		//设置按钮鼠标悬浮颜色
		void SetBtnHotColor(DWORD dwColor);
		//设置按钮按下状态颜色
		void SetBtnPushColor(DWORD dwColor);
		//设置进度条宽度
		void SetProgressWidth(DWORD dwWidth);
		//设置进度条半径
		void SetProgressRadius(DWORD dwRadius);
		//设置按钮半径
		void SetBtnRadius(DWORD dwRadius);
		//设置按钮与进度条间距
		void SetBtnSpace(DWORD dwSpace);
		//设置进度条值
		void SetScoreValue(DWORD dwValue);
		//设置按钮文本
		void SetBtnTextValue(LPCTSTR pstrText);
		//设置按钮文本颜色
		void SetBtnTextColor(DWORD dwValue);
		//获取按钮文本
		CDuiString GetBtnText() const;
		//设置进度条文本显示边距
		void SetValueTextPadding(RECT rc);
		//设置按钮文本显示边距
		void SetBtnTextPadding(RECT rc);
		//设置进度条字体
		void SetValueFont(int index);
		//设置按钮文本字体
		void SetBtnTextFont(int index);
		//获取进度条字体
		int GetValueFont() const;
		//获取按钮文本字体
		int GetBtnTextFont() const;
		//按钮与进度条空白处条件距离参数
		void SetBtnSpacePadding(DWORD dwValue);
		//设置圆形按钮禁用时字体颜色
		void EnableBtnStatus(bool bEnable);
		//判断鼠标点击按钮位置是否在圆形按钮中
		bool IsPointInBtn(POINT pMouse);
		void		SetEnabledValueStroke(bool _EnabledStroke);
		bool		GetEnabledValueStroke();
		void		SetEnabledValueShadow(bool _EnabledShadowe);
		bool		GetEnabledValueShadow();

		void		SetEnabledBtnTextStroke(bool _EnabledStroke);
		bool		GetEnabledBtnTextStroke();
		void		SetEnabledBtnTextShadow(bool _EnabledShadowe);
		bool		GetEnabledBtnTextShadow();

		void		SetStrokeValueColor(DWORD _StrokeColor);
		DWORD		GetStrokeValueColor();

		void		SetStrokeBtnTextColor(DWORD _StrokeColor);
		DWORD		GetStrokeBtnTextColor();

		//绘制进度条按钮控件
		void DoDrawProgress(HDC hdc, RECT CtrlRect);

		//绘制进度条数值
		void DoDrawScoreValue(HDC hdc);
		//绘制圆形控件文本
		void DoDrawBtnText(HDC hdc);

	private:
		ULONG_PTR				m_gdiplusToken;
		GdiplusStartupInput		m_gdiplusStartupInput;


		//内圆进度条圆心坐标
		PointF					m_pfCircularCenter;
		PointF					m_pfBtnCenter;

		//进度条颜色
		DWORD					m_dwProgressBkColor;
		//按钮外环颜色
		DWORD					m_dwOutBtnBkColor;
		//按钮背景颜色
		DWORD					m_dwBtnBkColor;
		//按钮hot状态颜色
		DWORD					m_dwBtnHotColor;
		//按钮Push状态颜色
		DWORD					m_dwBtnPushColor;
		//进度条宽度
		DWORD					m_dwProgressWidth;
		//进度条圆半径
		DWORD					m_dwProgressRadius;
		//按钮半径
		DWORD					m_dwBtnRadius;
		//按钮与进度条间隔
		DWORD					m_dwBtnSpace;
		//按钮控件的矩形区域，用于判断鼠标点击事件
		RECT					m_rcButton;

		//进度条显示数值
		DWORD					m_dwProgressValue;
		//文本显示样式
		UINT					m_uValueStyle;
		//文本显示位置
		RECT					m_rcValueTextPadding;
		//进度条数值颜色
		DWORD					m_dwVauleColor;
		//进度条数值禁用颜色
		DWORD					m_dwDisabledVauleColor;
		//按钮显示文本
		CDuiString				m_csBtnText;
		//文本显示样式
		UINT					m_uTextStyle;
		//文本显示位置
		RECT					m_rcBtnTextPadding;
		//按钮文本颜色
		DWORD					m_dwBtnTextColor;
		//按钮禁用文本颜色
		DWORD					m_dwDisabledBtnTextColor;
		//按钮与进度条空白处条件距离参数
		DWORD					m_dwBtnSpacePadding;

		int						m_hAlign;
		int						m_vAlign;

		int						m_iScoreFont;  //分数字体
		int						m_iBtnTextFont; //按钮文本字体
		bool					m_EnableEffect;

		RectF					m_ShadowValueOffset;
		RectF					m_ShadowBtnTextOffset;

		bool					m_EnabledValueStroke;
		bool					m_EnabledValueShadow;

		bool					m_EnabledBtnTextStroke;
		bool					m_EnabledBtnTextShadow;

		DWORD					m_dwStrokeValueColor;
		DWORD					m_dwStrokeBtnTextColor;
	};
}
#endif