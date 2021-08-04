#include "StdAfx.h"
#include <zmouse.h>

namespace DuiLib {
    extern CAnimationSpooler m_anim;
    HPEN m_hPens[UICOLOR__LAST] = { 0 };
    HFONT m_hFonts[UIFONT__LAST] = { 0 };
    HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
    LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
    COLORREF m_clrColors[UICOLOR__LAST][2] = { 0 };
    TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };
    HIMAGELIST m_himgIcons16 = NULL;
    HIMAGELIST m_himgIcons24 = NULL;
    HIMAGELIST m_himgIcons32 = NULL;
    HIMAGELIST m_himgIcons50 = NULL;
#define IDB_ICONS16 200
#define IDB_ICONS24 201
#define IDB_ICONS32 202
#define IDB_ICONS50 203

    bool CPaintManagerUI::AddAnimationJob( const CDxAnimationUI& _DxAnimationUI )
    {
        try
        {
            CDxAnimationUI* pDxAnimationJob = new CDxAnimationUI(_DxAnimationUI);
            if( pDxAnimationJob == NULL ) return false;
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
            bool nRet = m_anim.AddJob(pDxAnimationJob);
            return nRet;
        }
        catch (...)
        {
            throw "CPaintManagerUI::AddAnimationJob";
        }
    }

    HPEN CPaintManagerUI::GetThemePen( UITYPE_COLOR Index ) const
    {
        try
        {
            if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
            if( m_hPens[Index] == NULL ) m_hPens[Index] = ::CreatePen(PS_SOLID, 1, m_clrColors[Index][0]);
            return m_hPens[Index];
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemePen";
        }
    }

    HFONT CPaintManagerUI::GetThemeFont( UITYPE_FONT Index ) const
    {
        try
        {
            if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return NULL;
            if( m_hFonts[Index] == NULL ) m_hFonts[Index] = ::CreateFontIndirect(&m_aLogFonts[Index]);
            return m_hFonts[Index];
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeFont";
        }
    }

    HBRUSH CPaintManagerUI::GetThemeBrush( UITYPE_COLOR Index ) const
    {
        try
        {
            if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
            if( m_hBrushes[Index] == NULL ) m_hBrushes[Index] = ::CreateSolidBrush(m_clrColors[Index][0]);
            return m_hBrushes[Index];
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeBrush";
        }
    }

    COLORREF CPaintManagerUI::GetThemeColor( UITYPE_COLOR Index ) const
    {
        try
        {
            if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return RGB(0,0,0);
            return m_clrColors[Index][0];
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeColor";
        }
    }

    HICON CPaintManagerUI::GetThemeIcon( int Index, int cxySize ) const
    {
        try
        {
            if( m_himgIcons16 == NULL ) {
                m_himgIcons16 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
                m_himgIcons24 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
                m_himgIcons32 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
                m_himgIcons50 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS50), 50, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
            }
            if( cxySize == 16 ) return ImageList_GetIcon(m_himgIcons16, Index, ILD_NORMAL);
            else if( cxySize == 24 ) return ImageList_GetIcon(m_himgIcons24, Index, ILD_NORMAL);
            else if( cxySize == 32 ) return ImageList_GetIcon(m_himgIcons32, Index, ILD_NORMAL);
            else if( cxySize == 50 ) return ImageList_GetIcon(m_himgIcons50, Index, ILD_NORMAL);
            return NULL;
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeIcon";
        }
    }

    const TEXTMETRIC& CPaintManagerUI::GetThemeFontInfo( UITYPE_FONT Index ) const
    {
        try
        {
            if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return m_aTextMetrics[0];
            if( m_aTextMetrics[Index].tmHeight == 0 ) {
                HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, GetThemeFont(Index));
                ::GetTextMetrics(m_hDcPaint, &m_aTextMetrics[Index]);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
            return m_aTextMetrics[Index];
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeFontInfo";
        }
    }

    bool CPaintManagerUI::GetThemeColorPair( UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2 ) const
    {
        try
        {
            if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return false;
            clr1 = m_clrColors[Index][0];
            clr2 = m_clrColors[Index][1];
            return true;
        }
        catch (...)
        {
            throw "CPaintManagerUI::GetThemeColorPair";
        }
    }






    bool CPaintManagerUI::AddEffectsStyle( LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue )
    {
        CDuiString* pEffectsStyleValue = new CDuiString(pStrStyleValue);

        if(pEffectsStyleValue != NULL)
        {
            if( m_mEffectsStyle.Find(pStrStyleName) == NULL )
                m_mEffectsStyle.Set(pStrStyleName,pEffectsStyleValue);
            else
                delete pEffectsStyleValue;
            pEffectsStyleValue = NULL;
            return true;
        }
        return false;
    }

    CDuiString CPaintManagerUI::GetEffectsStyle( LPCTSTR pStrStyleName )
    {
        CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
        // todo fix  if( !pEffectsStyleValue && m_pParentResourcePM ) return m_pParentResourcePM->GetEffectsStyle(pStrStyleName);

        if( pEffectsStyleValue ) return pEffectsStyleValue->GetData();
        else return CDuiString(_T(""));
    }

    const CStdStringPtrMap& CPaintManagerUI::GetEffectsStyles() const
    {
        return m_mEffectsStyle;
    }

    bool CPaintManagerUI::RemoveEffectStyle( LPCTSTR pStrStyleName )
    {
        CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
        if( !pEffectsStyleValue ) return false;

        delete pEffectsStyleValue;
        return m_mEffectsStyle.Remove(pStrStyleName);
    }

    void CPaintManagerUI::RemoveAllEffectStyle()
    {
        CDuiString* pEffectsStyleValue;
        for( int i = 0; i< m_mEffectsStyle.GetSize(); i++ ) {
            if(LPCTSTR key = m_mEffectsStyle.GetAt(i)) {
                pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(key));
                delete pEffectsStyleValue;
            }
        }
    }


	
} // namespace DuiLib
