#include "StdAfx.h"

#ifdef QkListView
#include "core/InsituDebug.h"
#include "math.h"


namespace DuiLib {

    LRESULT CALLBACK WndProcInVis(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_SIZE:
            return 1;
        case WM_DESTROY:
            break;
        case WM_ERASEBKGND:
            return 1;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        return 0;
    }

    BOOL regWndClassInVisible(LPCTSTR lpcsClassName, DWORD dwStyle)
    {
        WNDCLASS wndclass = { 0 };

        wndclass.style = dwStyle;
        wndclass.lpfnWndProc = WndProcInVis;
        wndclass.cbClsExtra = 200;
        wndclass.cbWndExtra = 200;
        wndclass.hInstance = ::GetModuleHandle(NULL);
        wndclass.hIcon = NULL;
        //wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = lpcsClassName;

        ::RegisterClass(&wndclass);
        return TRUE;
    }

    IMPLEMENT_QKCONTROL(ListView)
       
    ListView::ListView()
        : CContainerUI()
        , _adapter(0)
        , m_HiddenItem(0)
        , _bHasBasicViewAdapter(false)
        , m_first_visible_index(0)
        , m_first_itemview_top_offset(0)
        , m_line_height(0)
        , m_total_height(0)
        , m_data_updated(0)
        , m_available_height(0)
        , _scrollYProxy(0)
        , _scrollPositionY(0)
        , _lastScrollPositionY(0)
        , _scrollOffsetY(0)
        , _heteroHeight(false)
        , _headerView(nullptr)
    {
        _bUseSmoothScroll = true;

        // 列表配置
        m_ListInfo.nColumns = 0;
        m_ListInfo.nFont = -1;
        m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE;
        m_ListInfo.dwTextColor = 0xFF000000;
        m_ListInfo.dwBkColor = 0;
        m_ListInfo.bAlternateBk = false;
        m_ListInfo.dwSelectedTextColor = 0xFF000000;
        m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
        m_ListInfo.dwHotTextColor = 0xFF000000;
        m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
        m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
        m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
        m_ListInfo.dwLineColor = 0;
        m_ListInfo.bShowRowLine = false;
        m_ListInfo.bShowColumnLine = false;
        m_ListInfo.bShowHtml = false;
        m_ListInfo.bMultiExpandable = false;
        m_ListInfo.bRSelected = false;
        ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
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

    void ListView::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        //if (_tcsicmp(pstrName, _T("scrollselect")) == 0) SetScrollSelect(_tcsicmp(pstrValue, _T("true")) == 0);
        if (_tcsnicmp(pstrName, _T("item"), 4)==0)
        {
            if (_tcsicmp(pstrName, _T("itemautoheight")) == 0) _heteroHeight = (_tcsicmp(pstrValue, _T("true")) == 0);
            else if (_tcsicmp(pstrName, _T("itemfont")) == 0) m_ListInfo.nFont = _ttoi(pstrValue);
            else if (_tcsicmp(pstrName, _T("itemalign")) == 0) {
                if (_tcsstr(pstrValue, _T("left")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
                    m_ListInfo.uTextStyle |= DT_LEFT;
                }
                if (_tcsstr(pstrValue, _T("center")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
                    m_ListInfo.uTextStyle |= DT_CENTER;
                }
                if (_tcsstr(pstrValue, _T("right")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
                    m_ListInfo.uTextStyle |= DT_RIGHT;
                }
            }
            else if (_tcsicmp(pstrName, _T("itemvalign")) == 0) {
                if (_tcsstr(pstrValue, _T("top")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_VCENTER | DT_BOTTOM);
                    m_ListInfo.uTextStyle |= DT_TOP;
                }
                if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM | DT_WORDBREAK);
                    m_ListInfo.uTextStyle |= DT_VCENTER | DT_SINGLELINE;
                }
                if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
                    m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
                    m_ListInfo.uTextStyle |= DT_BOTTOM;
                }
            }
            else if (_tcsicmp(pstrName, _T("itemendellipsis")) == 0) {
                if (_tcsicmp(pstrValue, _T("true")) == 0) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
                else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
            }
            else if (_tcsicmp(pstrName, _T("itemtextpadding")) == 0) STR2Rect(pstrValue, m_ListInfo.rcTextPadding);
            else if (_tcsicmp(pstrName, _T("itemtextcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwTextColor);
            else if (_tcsicmp(pstrName, _T("itembkcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwBkColor);
            else if (_tcsicmp(pstrName, _T("itembkimage")) == 0) m_ListInfo.sBkImage=pstrValue;

            else if (_tcsicmp(pstrName, _T("itemaltbk")) == 0) m_ListInfo.bAlternateBk=_tcsicmp(pstrValue, _T("true"))==0;
            else if (_tcsicmp(pstrName, _T("itemselectedtextcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwSelectedTextColor);
            else if (_tcsicmp(pstrName, _T("itemselectedbkcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwSelectedBkColor);
            else if (_tcsicmp(pstrName, _T("itemselectedimage")) == 0) m_ListInfo.sSelectedImage=pstrValue;
            else if (_tcsicmp(pstrName, _T("itemhottextcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwHotTextColor);
            else if (_tcsicmp(pstrName, _T("itemhotbkcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwHotBkColor);
            else if (_tcsicmp(pstrName, _T("itemhotimage")) == 0) m_ListInfo.sHotImage=pstrValue;
            else if (_tcsicmp(pstrName, _T("itemdisabledtextcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwDisabledTextColor);
            else if (_tcsicmp(pstrName, _T("itemdisabledbkcolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwDisabledBkColor);
            else if (_tcsicmp(pstrName, _T("itemdisabledimage")) == 0) m_ListInfo.sDisabledImage=pstrValue;
            else if (_tcsicmp(pstrName, _T("itemlinecolor")) == 0) STR2ARGB(pstrValue, m_ListInfo.dwLineColor);
            else if (_tcsicmp(pstrName, _T("itemshowrowline")) == 0) m_ListInfo.bShowRowLine = (_tcsicmp(pstrValue, _T("true")) == 0);
            else if (_tcsicmp(pstrName, _T("itemshowcolumnline")) == 0) m_ListInfo.bShowColumnLine = (_tcsicmp(pstrValue, _T("true")) == 0);
            else if (_tcsicmp(pstrName, _T("itemshowhtml")) == 0) m_ListInfo.bShowHtml = (_tcsicmp(pstrValue, _T("true")) == 0);
            else if (_tcsicmp(pstrName, _T("itemrselected")) == 0) m_ListInfo.bRSelected = (_tcsicmp(pstrValue, _T("true")) == 0);
        }
        else __super::SetAttribute(pstrName, pstrValue);
    }

    LRESULT ListView::GetAttribute(LPCTSTR pstrName, LPARAM lParam, WPARAM wParam)
    {
        if( _tcsicmp(pstrName, _T("ListInfo")) == 0 ) {
            return (LRESULT)&m_ListInfo;
        }
        return 0;
    }

    void ListView::SetAdapter(ListViewAdapter* adapter)
    {
        if (_adapter!=adapter)
        {
            _adapter = adapter;
            _bHasBasicViewAdapter = dynamic_cast<ListBasicViewAdapter*>(adapter);
            if (m_HiddenItem)
            {
                delete m_HiddenItem;
                CControlUI *pControl;
                for (size_t i = 0, length=m_items.GetSize(); i < length; i++)
                {
                    pControl = static_cast<CControlUI*>(m_items.GetAt(i));
                    pControl->SetAutoDestroy(true);
                    delete pControl;
                }
                for (size_t i = 0, length=_recyclePool.GetSize(); i < length; i++)
                {
                    pControl = static_cast<CControlUI*>(_recyclePool.GetAt(i));
                    pControl->SetAutoDestroy(true);
                    delete pControl;
                }
                m_items.Empty();
                _recyclePool.Empty();
            }
            if (adapter)
            {
                m_HiddenItem = adapter->CreateItemView(this, 0);
                if (m_HiddenItem)
                {
                    auto tmp = new CControlUI; 
                    tmp->SetFixedHeight(m_HiddenItem->GetFixedHeight());
                    //m_HiddenItem = tmp; // ??? 在干啥勒 忘记了
                    if( _manager ) _manager->InitControls(m_HiddenItem, this);
                    NeedUpdate();
                }
            }
        }
    }

    bool ListView::SetHeaderView(CControlUI* view)
    {
        CListHeaderUI* headerview = dynamic_cast<CListHeaderUI*>(view);
        if (headerview)
        {
            if (_headerView && _headerView != headerview)
            {
                //__super::Remove(_headerView);
                // todo
            }
            _headerView = headerview;
            _manager->InitControls(_headerView, this);
            m_ListInfo.headerView = headerview;
            m_ListInfo.nColumns = headerview->GetCount();
            _headerViewInit = false;
            return true;
        }
        //if( IsVisible() ) NeedUpdate();
        return false;
    }

    CControlUI* ListBasicViewAdapter::GetItemAt(int position) 
    {
        CControlUI* ret = static_cast<CControlUI*>(m_items.GetAt(position));
        CPaintManagerUI* manager = _parent->GetManager();
        if (manager && (ret->GetParent()!=_parent || ret->GetManager()!=manager))
        {
            manager->InitControls(ret, _parent);
        }
        return ret;
    }

    void ListBasicViewAdapter::AddView(CControlUI* view, int index)
    {
        int size = m_items.GetSize();
        if (index==-1) index=size;
        else if (index<0) index=0;
        else if (index>size) index=m_items.GetSize();
        if (index==size)
            m_items.Add(view);
        else
            m_items.InsertAt(index, view);
    }

    bool ListView::Add(CControlUI* pControl)
    {
        return AddAt(pControl, -1);
    }
    
    bool ListView::AddAt(CControlUI* pControl, int iIndex)
    {
        if (pControl)
        {
            if (pControl->GetInterface(_T("ListHeader")) != NULL) 
            {
                return SetHeaderView(pControl);
            }
            if (!_adapter)
            {
                SetAdapter(new ListBasicViewAdapter(this));
                // todo
                if (!m_HiddenItem)
                {
                    m_HiddenItem = pControl;
                    if(_manager) _manager->InitControls(m_HiddenItem, this);
                }
            }
            if (_bHasBasicViewAdapter)
            {
                //LogIs("ListView::添加视图::name=%s, text=%s, mng=%d", (LPCTSTR)pControl->GetName(), (LPCTSTR)pControl->GetText(), _manager);
                dynamic_cast<ListBasicViewAdapter*>(_adapter)->AddView(pControl, iIndex);
                if(_manager) _manager->InitControls(pControl, this);
                if( IsVisible() ) NeedUpdate();

                IListItemUI* listItemView = dynamic_cast<IListItemUI*>(pControl);
                if (listItemView)
                {
                    listItemView->SetOwner(this);
                }
                return true;
            }
        }
        return false;
    }

    void ListView::DoScroll(int x, int y)
    {
        int size = m_items.GetSize();
        if (size)
        {
            // 实施滚动
            int newOffsetY = _scrollOffsetY + y;
            CControlUI* firstVisible = static_cast<CControlUI*>(m_items[0]);
            int itemHeight = firstVisible->GetHeight();

            if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) 
                _scrollX = m_pHorizontalScrollBar->GetScrollPos();

            boolean heteroHeight = _heteroHeight;//_heteroHeight;

            if (newOffsetY<0 || newOffsetY>=itemHeight)
            {
                SIZE szAvailable = { m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top };
                SIZE estSz;
                int index = 0;
                int maxIndex = _adapter->GetItemCount()-1;
                if (!heteroHeight)
                {
                    estSz = m_HiddenItem->EstimateSize(szAvailable);
                    if (estSz.cy==0) estSz.cy=30;
                    itemHeight = estSz.cy;
                    maxIndex = maxIndex - ceil(double(m_available_height) / estSz.cy) + 1;
                    if (maxIndex < 0) maxIndex = 0;
                }
                if (itemHeight && abs(y)/itemHeight>=10)
                {
                    index = y/itemHeight;
                    newOffsetY = 0;
                    LogIs("ListView::DoScroll::快速滚动 delta=%d", index, _scrollPositionY + index);
                    //_scrollPositionY = (std::max)(0, (std::min)(maxIndex, _scrollPositionY + y/itemHeight));
                }
                else if (newOffsetY<0)
                {
                    LogIs("ListView::DoScroll::切换至上N个", _scrollPositionY, index, _scrollPositionY + index);
                    while(newOffsetY<0
                        && _scrollPositionY + index > 0
                        && index>-1024)
                    {
                        --index;
                        //todo !!! 
                        if (heteroHeight) 
                        {
                            firstVisible = m_HiddenItem;
                            _adapter->OnBindItemView(firstVisible, _scrollPositionY + index);
                            estSz = firstVisible->EstimateSize(szAvailable);
                            itemHeight = estSz.cy;
                        }
                        newOffsetY += itemHeight;
                    }
                }
                else // newOffsetY>=itemHeight
                {
                    LogIs("ListView::DoScroll::切换至下N个", _scrollPositionY, index, _scrollPositionY + index);
                    while(newOffsetY >= itemHeight
                        && _scrollPositionY + index < maxIndex
                        && index<1024
                        )
                    {
                        ++index;
                        newOffsetY -= itemHeight;
                        //todo !!! 
                        if (heteroHeight) 
                        {
                            if (index>=size)
                            {
                                firstVisible = m_HiddenItem;
                                _adapter->OnBindItemView(firstVisible, _scrollPositionY + index);
                                estSz = firstVisible->EstimateSize(szAvailable);
                                itemHeight = estSz.cy;
                            }
                            else
                            {
                                firstVisible = static_cast<CControlUI*>(m_items[index]);
                                itemHeight = firstVisible->GetHeight();
                            }
                        }
                    }
                }
                if (!heteroHeight)
                {
                    if (_scrollPositionY + index > maxIndex) 
                        index = maxIndex - _scrollPositionY;
                    if (_scrollPositionY + index < 0) 
                        index = - _scrollPositionY;
                    if (newOffsetY>itemHeight)  newOffsetY = ceil(double(m_available_height) / estSz.cy) * estSz.cy - GetHeight();
                    if (newOffsetY<0)  newOffsetY = 0;
                }
                else if(newOffsetY<0 || newOffsetY>itemHeight)
                {
                    newOffsetY=0;
                }
                LogIs("ListView::DoScroll::, pos=%d delta=%d/%d res=%d", _scrollPositionY, index, y, _scrollPositionY + index);
                _scrollPositionY += index;
                _scrollPositionY = (std::max)(0, (std::min)(maxIndex, _scrollPositionY));
            }
            _scrollOffsetY = newOffsetY;
            //Invalidate();
            NeedUpdate();

            // PostUpdate
            //SetTimer(0x112, 10, true);
        }
    }

    static INT64 TicksLastDraw, TicksPerSecond;

    UINT_PTR timer_smooth_scroll=0x123;
    int timer_smooth_scroll_interval=10;

    bool ListView::SetScrollPos(SIZE szPos, bool bMsg, bool seeking)
    {
        int cx = 0;
        int cy = 0;
        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
            int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
            m_pVerticalScrollBar->SetScrollPos(szPos.cy);
            cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
        }

        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
            int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
            m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
            cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
        }
        if ((cx || cy) && 0)
        {
            ::ScrollWindowEx(GetHWND(), cx, -cy, 0, 0, 0, 0, SW_SCROLLCHILDREN);
            return true;
        }
        //if (cy && m_items.GetSize())
        //{
        //    bool scroll=false;
        //    if(cy<0) {
        //        CControlUI* pControl = (CControlUI*)m_items.GetAt(0);
        //        if(pControl->GetPos().top+cy>-pControl->GetHeight()
        //            &&) {
        //            scroll = true;
        //        }
        //    } 
        //    if(scroll) {
        //        ::ScrollWindowEx(GetHWND(), cx, -cy, 0, 0, 0, 0, SW_SCROLLCHILDREN);
        //        return true;
        //    }
        //}
        if (cx || cy)
        {
            //LogIs("SetScrollPos:: %d, %d, fY=%d", cx, cy, _scrollY + cy);
            //LogIs("SetScrollPos:: %d, %d  %d/%d", szPos.cx, szPos.cy, m_pVerticalScrollBar->GetScrollPos(), m_pVerticalScrollBar->GetScrollRange());
            if (_bUseSmoothScroll) // _bUseSmoothScroll
            {
                _scrollX += cx;
                _scrollY += cy;
                //::QueryPerformanceFrequency((LARGE_INTEGER*)&TicksPerSecond);
                //::QueryPerformanceCounter((LARGE_INTEGER*)&TicksLastDraw);
                //LogIs("TicksPerSecond=%d", TicksPerSecond);
                if(!_smoothScrolling) {
                    _smoothScrolling = true;
                    _scrollSpeed = 6;
                    SetTimer(timer_smooth_scroll, timer_smooth_scroll_interval, true);
                } 
                else {
                    float spd = 5;
                    float spdFac = fabs(_scrollY/35)/fmax(3, 1);
                    if (spdFac>1)
                        spd *= spdFac;
                    if (spd>_scrollSpeed)
                    {
                        _scrollSpeed = spd;
                    }
                }
                _seeking = seeking;
            }
            else
            {
                DoScroll(0, cy);
                //NeedUpdate();
            }
            return true;
        }
        return false;
    }

    void ListView::DoEvent(TEventUI& event)
    {
        if( event.Type == UIEVENT_TIMER ) 
        {
            if (event.wParam==timer_smooth_scroll) //  && _bUseSmoothScroll
            {
                int scrollY = 0;
                if (_scrollY)
                {
                    INT64 tk;
                    scrollY = _scrollY*10*1.2/25;
                    //scrollY = 1;
                    scrollY = _scrollY>0?_scrollSpeed:-_scrollSpeed;
                    if (_seeking  )
                    {
                        scrollY = _scrollY*10*1.2/25;
                    }
                    if (std::abs(scrollY)<1)
                    {
                        scrollY = _scrollY>0?1:-1;
                    }
                    if ((_scrollY-scrollY>0) ^ (_scrollY>0))
                    {
                        scrollY = _scrollY;
                        _scrollY = 0;
                    }
                    else
                    {
                        _scrollY -= scrollY;
                    }
                }
                //if(scrollY*_scrollY<0) LogIs("213213");
                if(scrollY)
                    DoScroll(0, scrollY);
                if (!_scrollY)
                {
                    KillTimer(event.wParam);
                    _smoothScrolling = false;
                   // _seeking = false;
                }
            }
            return;
        }
        __super::DoEvent(event);
    }

    //bool ListView::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
    //{
    //    if(_headerView) m_items.Add(_headerView);
    //    bool ret = __super::DoPaint(hDC, rcPaint, pStopControl);
    //    if(_headerView) m_items.RemoveAt(m_items.GetSize()-1);
    //    return ret;
    //}

    SIZE ListView::EstimateSize(const SIZE & szAvailable)
    {
        if (1) return __super::EstimateSize(szAvailable);
        //if (1) return {100,100};
        int cxFixed = 0;
        int cyFixed = 0;
        for( int it = 0; it < GetCount(); it++ ) {
            CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
            if( !pControl->IsVisible() ) continue;
            SIZE sz = pControl->EstimateSize(szAvailable);
            cyFixed += sz.cy;
            if( cxFixed < sz.cx )
                cxFixed = sz.cx;
        }

        //for (int it = 0; it < GetCount(); it++) {
        //    CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
        //    if (!pControl->IsVisible()) continue;
        //
        //    pControl->SetFixedWidth(MulDiv(cxFixed, 100, GetManager()->GetDPIObj()->GetScale()));
        //}

        return {cxFixed, cyFixed};
    }
    HWND _hLeeverse=0;
    void ListView::SetPos(RECT rc, bool bNeedInvalidate) {
        if (!_hLeeverse && _hParent)
        {
            regWndClassInVisible(L"InVisible", CS_HREDRAW | CS_VREDRAW);
            _hLeeverse = ::CreateWindowEx(0 , L"InVisible" , NULL
                , WS_CHILD , 0 , 0 , 0 , 0 , _hParent , NULL , ::GetModuleHandle(NULL), NULL);

        }
        CControlUI::SetPos(rc, bNeedInvalidate);
        if (!_adapter) return;
        rc = m_rcItem;
        ApplyInsetToRect(rc);

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

        int top = rc.top - _scrollOffsetY;
        RECT rcCtrl = m_rcItem;
        int hhead = 0;
        if (_headerView)
        {
            if (!_headerViewInit)
            {
                _manager->InitControls(_headerView, this);
            }
            int index = m_items.FindEx(_headerView, -1, -1, true);
            if(index>=0) m_items.Remove(index);
            SIZE szHeader = _headerView->EstimateSize(szAvailable);
            hhead = szHeader.cy;
            top += hhead;
            szAvailable.cy -= hhead;
            rcCtrl.bottom = rcCtrl.top + hhead;
            _headerView->SetPos(rcCtrl);
        }

        m_available_width = szAvailable.cx;
        m_available_height = szAvailable.cy;

        boolean heteroHeight = _heteroHeight;//_heteroHeight;

        int maxIndex = _adapter->GetItemCount()-1;
        SIZE estSz;
        if (!heteroHeight)
        {
            estSz = m_HiddenItem->EstimateSize(szAvailable);
            if (estSz.cy==0) estSz.cy=30;
            // 提前贴底重排（简化处理）
            int maxIndexFixed = maxIndex - ceil(double(m_available_height) / estSz.cy) + 1;
            if (maxIndexFixed < 0) maxIndexFixed = 0;
            if (_scrollPositionY > maxIndexFixed)
            {
                _scrollPositionY = maxIndexFixed;
            }
        }

        // place children with different height.
        // reuse at same index, recycle if unused, recreate otherwise. 
        int bkRecyclableCnt = 0;
        int fwRecyclable = m_items.GetSize();
        int delta = _scrollPositionY-_lastScrollPositionY;
        int reusableSt = delta>=0?delta:0;
        int reusableCnt = fwRecyclable - reusableSt;
        int reusableEd = -1;
        if (reusableCnt<=0)
        {
            reusableCnt = 0;
            reusableSt = -1;
        }
        else if (reusableCnt>0)
        {
            bkRecyclableCnt = reusableSt;
            reusableEd = reusableSt+reusableCnt-1;
        }

        int index=0;
        int delta_index=delta;

        CStdPtrArray children = m_items;
        m_items.Empty();

        CControlUI *pControl;
        bool resued;
        while(top<rc.bottom 
            && m_available_height > 0
            && _scrollPositionY + index <= maxIndex 
            && index<4096)
        {
            // 是否复用了相同位置的视图（无需重新绑定）
            resued = false;
            if (reusableCnt>0 && delta_index>=reusableSt && delta_index<=reusableEd)
            {
                resued = true;
                pControl = static_cast<CControlUI*>(children.GetAt(delta_index));
                reusableCnt--;
                reusableSt = delta_index+1;
            }
            else if(_bHasBasicViewAdapter)
            {
                pControl = static_cast<ListBasicViewAdapter*>(_adapter)->GetItemAt(_scrollPositionY + index);
            }
            else if(_recyclePool.GetSize()>0)
            {
                pControl = static_cast<CControlUI*>(_recyclePool.RemoveAt(_recyclePool.GetSize()-1));
              //  ShowWindow(pControl->GetHWND(), SW_SHOWNOACTIVATE);
                //if()
                //SetParent(pControl->GetHWND(), _hWnd);
               // SetWindowPos(pControl->GetHWND(), HWND_BOTTOM, 0,0,0,0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOREPOSITION);
            }
            else if(bkRecyclableCnt>0)
            {
                pControl = static_cast<CControlUI*>(children.GetAt(--bkRecyclableCnt));
            }
            else if (fwRecyclable-1>reusableSt) // fwRecyclable>=0 && 
            {
                pControl = static_cast<CControlUI*>(children.GetAt(fwRecyclable-1));
                if (reusableCnt>0 && fwRecyclable-1<=reusableEd)
                {
                    reusableEd--;
                    reusableCnt--;
                }
                fwRecyclable--;
            }
            else
            {
                pControl = _adapter->CreateItemView(this, 0);
                ASSERT(pControl);
                // todo check not null
                if( _manager ) _manager->InitControls(pControl, this);
            }

            m_items.Add(pControl);
            if (!resued)
            {
                _adapter->OnBindItemView(pControl, _scrollPositionY + index);
            }

            if (heteroHeight)
                estSz = pControl->EstimateSize(szAvailable);

            if (_headerView)
            {
                rcCtrl = { rc.left, top, rc.right, top + estSz.cy };
                ApplyMultiColumn(pControl, rcCtrl);
            }
            else 
            {
                pControl->SetPos({ rc.left, top, rc.right, top + estSz.cy }, false);
            }
            //LogIs("ListView::子项高度计算@%d::%d", _scrollPositionY + index, estSz.cy);

            top += estSz.cy;
            //szAvailable.cy -= estSz.cy;
            index++;
            delta_index++;
        }
        // 贴底重排，子项充足时，避免最后一项的底部还有空间
        //if(false)
        if (heteroHeight && top<rc.bottom && _scrollPositionY>0)
        {
            top += _scrollOffsetY;
            _scrollOffsetY = 0;
            while(top<rc.bottom && _scrollPositionY>0)
            {
                --_scrollPositionY;
                if(_bHasBasicViewAdapter)
                {
                    pControl = static_cast<ListBasicViewAdapter*>(_adapter)->GetItemAt(_scrollPositionY);
                }
                else if(_recyclePool.GetSize()>0)
                {
                    pControl = static_cast<CControlUI*>(_recyclePool.RemoveAt(_recyclePool.GetSize()-1));
                }
                else
                {
                    pControl = _adapter->CreateItemView(this, 0);
                    // todo check not null
                    if( _manager ) _manager->InitControls(pControl, this);
                }
                m_items.InsertAt(0, pControl);
                _adapter->OnBindItemView(pControl, _scrollPositionY);
                estSz = pControl->EstimateSize(szAvailable);

                top += estSz.cy;
            }
            if (top>rc.bottom)
            {
                _scrollOffsetY = top-rc.bottom;
            }
            int top = rc.top - _scrollOffsetY + hhead;
            for (size_t i = 0, length=m_items.GetSize(); i < length; i++)
            {
                pControl = static_cast<CControlUI*>(m_items.GetAt(i));
                estSz = pControl->EstimateSize(szAvailable);
                rcCtrl = { rc.left, top, rc.right, top + estSz.cy };
                ApplyMultiColumn(pControl, rcCtrl);
                top += estSz.cy;
            }
        }
            
        // 抓住滚动条时，尽量不更新滚动范围。
        short handleScroll = m_pVerticalScrollBar && !_manager->IsCaptured();
        if (!handleScroll && m_pVerticalScrollBar )
        {
            _scrollYProxy = m_pVerticalScrollBar->GetScrollPos();
            // 端部处理：移至最顶部或最底部，更新滚动条。
            //if(heteroHeight)
            if ((_scrollYProxy==0&&_scrollPositionY>0) || (_scrollYProxy>=m_pVerticalScrollBar->GetScrollRange() /*&& _scrollPositionY+m_items.GetSize()<=maxIndex*/))
            {
                // 可是，需要记录差值，否则跳过了很多。
                handleScroll = 2;
            }
        }
        //handleScroll = 1; // 会造成抖动
        if (handleScroll)
        {
            if (heteroHeight)
            {
                _avgHeight = m_items.GetSize();
                if (_avgHeight)
                {
                    _avgHeight = (top - rc.top + _scrollOffsetY) / _avgHeight;
                    m_total_height = _avgHeight*_adapter->GetItemCount();
                    _scrollYProxy = _avgHeight*_scrollPositionY + _scrollOffsetY;
                }
            }
            else
            {
                _avgHeight = estSz.cy;
                m_total_height = _avgHeight*_adapter->GetItemCount();
                _scrollYProxy = _avgHeight*_scrollPositionY + _scrollOffsetY;
            }
            //LogIs("SetPos --- %d   pos=%d avg=%d scr=%d", _scrollYProxy, _scrollPositionY, _avgHeight, _scrollY);
        }
        LONG thumb = handleScroll==2?m_pVerticalScrollBar->GetThumbPosition():0;
        ProcessScrollBar(szAvailable, 0, m_total_height);

        if (handleScroll==2)
        {
            // 记录差值，Y轴上的像素距离 
            // todo 提高精度
            m_pVerticalScrollBar->SetMouseBias(m_pVerticalScrollBar->GetThumbPosition()-thumb);
        }

        // 回收没有引用的子项
        //if(!_bHasBasicViewAdapter)
        if(0)
        while (true)
        {
            if (reusableCnt>0)
            {
                pControl = static_cast<CControlUI*>(children.GetAt(reusableSt));
                reusableCnt--;
                reusableSt++;
            }
            else if(bkRecyclableCnt>0)
            {
                pControl = static_cast<CControlUI*>(children.GetAt(--bkRecyclableCnt));
            }
            else if (fwRecyclable-1>reusableSt) // fwRecyclable>=0 && 
            {
                pControl = static_cast<CControlUI*>(children.GetAt(--fwRecyclable));
                if (reusableCnt>0 && fwRecyclable<=reusableEd)
                {
                    reusableEd--;
                    reusableCnt--;
                }
            }
            else break;
            // todo check not null
           // ShowWindow(pControl->GetHWND(), SW_HIDE);
           //if(_hLeeverse)SetParent(pControl->GetHWND(), _hLeeverse);

           // SetWindowPos(pControl->GetHWND(), HWND_TOP, 0,0,0,0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOREPOSITION);

            _recyclePool.Add(pControl);
        }
        //for (size_t i = 0; i < children.GetSize(); i++)
        //    if(m_items.Find(children.GetAt(i))<0)_recyclePool.Add(children.GetAt(i));
        _lastScrollPositionY = _scrollPositionY;

        if (_headerView)
        {
            m_items.Add(_headerView);
        }

       // LogIs("ListView::布局完毕::, pos=%d cnt=%d", _scrollPositionY, GetCount());
        if (GetCount())
        {
            RECT rc = GetItemAt(0)->GetPos();
            //LogIs("ListView::布局完毕::, rect=%d,%d,%d,%d", rc.left, rc.right, rc.top, rc.bottom);
        }
    }

    void ListView::ApplyMultiColumn(CControlUI* pControl, const RECT & rcItem)
    {
        CContainerUI* horLayout;
        if (_headerView && (horLayout = dynamic_cast<CContainerUI*>(pControl)))
        {
            RECT rcPart = rcItem;
            for (size_t i = 0, length=(std::min)(horLayout->GetCount(), _headerView->GetCount()); i < length; i++)
            {
                CControlUI* headerPart = _headerView->GetItemAt(i);
                const RECT & rcRef = headerPart->GetPos();
                rcPart.right = rcRef.right;

                horLayout->GetItemAt(i)->SetPos(rcPart);
                rcPart.left = rcPart.right;
            }
            horLayout->SupressChildLayouts(true);
            horLayout->SetPos(rcItem);
            horLayout->SupressChildLayouts(false);
        }
        else 
        {
            pControl->SetPos(rcItem);
        }
    }

    void ListView::ProcessScrollBar(SIZE szAvailable, int cxRequired, int cyRequired)
    {
        if (szAvailable.cy < cyRequired && m_pVerticalScrollBar) {
            RECT rcScrollBarPos = { m_rcItem.right - m_pVerticalScrollBar->GetFixedWidth(), 
                m_rcItem.top, 
                m_rcItem.right, 
                m_rcItem.bottom };
            if (szAvailable.cx < cxRequired && m_pHorizontalScrollBar)
                rcScrollBarPos.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
            m_pVerticalScrollBar->SetPos(rcScrollBarPos);
            if (_scrollYProxy > cyRequired - szAvailable.cy) {
                _scrollYProxy = cyRequired - szAvailable.cy;
            }
            m_pVerticalScrollBar->SetScrollRangeAndPos(cyRequired - szAvailable.cy, _scrollYProxy);
            m_pVerticalScrollBar->SetVisible(true);
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
            if (_scrollX > cxRequired - szAvailable.cx) {
                _scrollX = cxRequired - szAvailable.cx;
                m_pHorizontalScrollBar->SetScrollPos(_scrollX);
            }
            m_pHorizontalScrollBar->SetScrollRange(cxRequired - szAvailable.cx);
            m_pHorizontalScrollBar->SetVisible(true);
        }
        else {
            if (m_pHorizontalScrollBar)
                m_pHorizontalScrollBar->SetVisible(false);
        }
    }


}

#endif