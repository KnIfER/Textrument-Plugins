#ifndef __UICIRCLE_PROGRESS_H__
#define __UICIRCLE_PROGRESS_H__

#pragma once

namespace DuiLib
{
    class CCircleProgressUI :
        public CProgressUI
    {
    public:
        CCircleProgressUI(void);
        ~CCircleProgressUI(void);

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetCircular(BOOL bCircular = TRUE);
        void SetClockwiseRotation(BOOL bClockwise = TRUE);
        void SetCircleWidth(DWORD dwCircleWidth);
        void SetBgColor(DWORD dwBgColor);
        DWORD GetBgColor() const;
        void SetFgColor(DWORD dwBgColor);
        DWORD GetFgColor() const;
        void SetIndicator(LPCTSTR lpIndicatorImage);
        void SetEnableCircleEffect(BOOL bEnableCircleEffect = FALSE);
        void SetCircleGradientColor1(DWORD dwColor);
        void SetCircleGradientColor2(DWORD dwColor);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        void PaintBkColor(HDC hDC);

    private:
        BOOL        m_bCircular;
        BOOL        m_bClockwise;
        DWORD       m_dwCircleWidth;
        DWORD       m_dwBgColor;
        DWORD       m_dwFgColor;
        BOOL        m_bEnableCircleEffect;
        DWORD       m_dwGradientColor1;
        DWORD       m_dwGradientColor2;
        Image*      m_pIndicator;
        CDuiString     m_sIndicatorImage;
    };

}       //namespace Duilib

#endif // !CIRCLE_PROGRESS_H
