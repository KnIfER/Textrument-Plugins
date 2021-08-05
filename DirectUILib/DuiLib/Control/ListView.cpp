#include "StdAfx.h"


namespace DuiLib {

    IMPLEMENT_DUICONTROL(ListView)
       
    ListView::ListView()
        : m_Delegate(0)
        , m_HiddenItem(0)
        , m_first_visible_index(0)
        , m_first_itemview_top_offset(0)
        , m_line_height(0)
        , m_total_height(0)
        , m_data_updated(0)
        , m_available_height(0)
        , m_ScrollY(0)
        , m_ScrollX(0)
    {
        
    }

    LPCTSTR ListView::GetClass() const
    {
        return _T("ListView");
    }

    LPVOID ListView::GetInterface(LPCTSTR pstrName)
    {
        if( _tcsicmp(pstrName, _T("ListView")) == 0 ) return static_cast<ListView*>(this);
        return CContainerUI::GetInterface(pstrName);
    }

    void ListView::SetAdapter(ListViewAdapter* adapter)
    {
        m_Delegate = adapter;
        NeedUpdate();
    }

    void ListView::SetScrollPos(SIZE szPos, bool bMsg)
    {
        __super::SetScrollPos(szPos, bMsg);

        if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) 
            m_ScrollX = m_pHorizontalScrollBar->GetScrollPos();
        if(m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) 
            m_ScrollY = m_pVerticalScrollBar->GetScrollPos();

        NeedUpdate();
    }

    void ListView::SetPos(RECT rc, bool bNeedInvalidate) {
        CControlUI::SetPos(rc);
        if (!m_Delegate) return;
        rc = m_rcItem;

        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        m_available_width = szAvailable.cx;
        m_available_height = szAvailable.cy;

        size_t item_view_count = ceil(double(m_available_height) / m_HiddenItem->GetFixedHeight()) + 1;
        if (m_Delegate->GetItemCount() < item_view_count)
            item_view_count = m_Delegate->GetItemCount();

        m_total_height = m_Delegate->GetItemCount() * m_HiddenItem->GetFixedHeight();
        int width_required = m_Delegate->GetItemCount() == 0 ? 0 : m_HiddenItem->GetFixedWidth();
        ProcessScrollBar(szAvailable, width_required, m_total_height);

        bool force_update = ProcessVisibleItems(item_view_count);
        UpdateSubviews(rc, force_update || m_data_updated);
    }

    bool ListView::ProcessVisibleItems(int item_view_count) {
        if (m_items.GetSize() != item_view_count) {
            if (m_items.GetSize() < item_view_count) {
                for (int i = m_items.GetSize(); i != item_view_count; ++i) {
                    CControlUI *pControl = m_Delegate->CreateItemView();
                    if (m_pManager != NULL) m_pManager->InitControls(pControl, this);
                    m_items.Add(pControl);
                    //pControl->SetInternVisible(true);
                    //Add(pControl);
                }
            }
            return true;
        }
        return false;
    }

    void ListView::ProcessScrollBar(SIZE szAvailable, int cxRequired, int cyRequired)
    {
        if (m_bScrollProcess)
            return;

        m_bScrollProcess = true;
        if (szAvailable.cy < cyRequired && m_pVerticalScrollBar) {
            RECT rcScrollBarPos = { m_rcItem.right - m_pVerticalScrollBar->GetFixedWidth(), 
                m_rcItem.top, 
                m_rcItem.right, 
                m_rcItem.bottom };
            if (szAvailable.cx < cxRequired && m_pHorizontalScrollBar)
                rcScrollBarPos.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
            m_pVerticalScrollBar->SetPos(rcScrollBarPos);
            if (m_ScrollY > cyRequired - szAvailable.cy) {
                m_ScrollY = cyRequired - szAvailable.cy;
                m_pVerticalScrollBar->SetScrollPos(m_ScrollY);
            }
            m_pVerticalScrollBar->SetScrollRange(cyRequired - szAvailable.cy);
        }
        else {
            if (m_pVerticalScrollBar)
                m_pVerticalScrollBar->SetVisible(false);
        }

        if (szAvailable.cx < cxRequired && m_pHorizontalScrollBar) {
            RECT rcScrollBarPos = { m_rcItem.left, 
                m_rcItem.bottom -  m_pHorizontalScrollBar->GetFixedHeight(),
                m_rcItem.right,
                m_rcItem.bottom};
            if (szAvailable.cy < cyRequired && m_pVerticalScrollBar)
                rcScrollBarPos.right -= m_pVerticalScrollBar->GetFixedWidth();
            m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
            if (m_ScrollX > cxRequired - szAvailable.cx) {
                m_ScrollX = cxRequired - szAvailable.cx;
                m_pHorizontalScrollBar->SetScrollPos(m_ScrollX);
            }
            m_pHorizontalScrollBar->SetScrollRange(cxRequired - szAvailable.cx);
        }
        else {
            if (m_pHorizontalScrollBar)
                m_pHorizontalScrollBar->SetVisible(false);
        }

        m_bScrollProcess = false;
    }


    void ListView::UpdateSubviews(RECT rc, bool force_update) {
        int item_view_height = m_HiddenItem->GetFixedHeight();
        int item_view_width = m_HiddenItem->GetFixedWidth();

        int scroll_posY = (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) ? m_ScrollY : 0;
        int first_visible_index = scroll_posY / item_view_height;
        int itemview_pos_top = scroll_posY % item_view_height;

        if (m_first_visible_index == first_visible_index && m_first_itemview_top_offset == itemview_pos_top && !force_update) {
            return;
        }

        m_first_visible_index = first_visible_index;
        m_first_itemview_top_offset = itemview_pos_top;

        if (m_first_itemview_top_offset > 0)
            m_first_itemview_top_offset = -m_first_itemview_top_offset;

        for (int i = 0; i != m_items.GetSize(); ++i) {
            CControlUI *pControl = static_cast<CControlUI*>(m_items.GetAt(i));
            if (first_visible_index + i >= m_Delegate->GetItemCount()) {
                pControl->SetVisible(false);
                continue;
            }
            pControl->SetVisible(true);
            //pControl->NeedUpdate();
            RECT rcCtrl = { rc.left - m_ScrollX,
                rc.top + m_first_itemview_top_offset,
                item_view_width == 0 ? rc.right : rc.left + item_view_width - m_ScrollX,
                rc.top + m_first_itemview_top_offset + item_view_height };
            pControl->SetPos(rcCtrl);
            m_first_itemview_top_offset += item_view_height;
            if (m_data_updated || m_itemview_index_map.find(pControl) == m_itemview_index_map.end() ||
                m_itemview_index_map[pControl] != first_visible_index + i) {
                m_itemview_index_map[pControl] = first_visible_index + i;
                m_Delegate->OnBindItemView(pControl, first_visible_index + i);
            }
        }
    }



}