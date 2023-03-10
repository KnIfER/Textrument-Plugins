#ifdef QkListView

#pragma once


namespace DuiLib {
    // 适配器接口
    class ListViewAdapter {
    public:
        virtual size_t GetItemCount() = 0;
        virtual CControlUI* CreateItemView(CControlUI* parent, int type) = 0;
        virtual void OnBindItemView(CControlUI* view, size_t index) = 0;
    };

    // 普通视图适配器。使用此类适配器，列表内部不回收视图。
    class ListView;
    class ListBasicViewAdapter : public ListViewAdapter
    {    
    public:
        ListBasicViewAdapter(ListView* parent){_parent=parent;}
        size_t GetItemCount() {
            return m_items.GetSize();
        };
        CControlUI* CreateItemView(CControlUI* view, int type) {
            return NULL;
        };
        CControlUI* GetItemAt(int position);
        void AddView(CControlUI* view, int index=-1);
        void OnBindItemView(CControlUI* view, size_t index) { }
    protected:
        CStdPtrArray m_items;
        ListView* _parent;
    };

    // 虚表ListView
    class UILIB_API ListView : public CContainerUI {

        DECLARE_QKCONTROL(ListView)

    public:
        ListView();

        ~ListView(){}

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        LRESULT GetAttribute(LPCTSTR pstrName, LPARAM lParam=0, WPARAM wParam=0);

        SIZE EstimateSize(const SIZE & szAvailable);
        void ComputeScroll() override;

        void SetPos(RECT rc, bool bNeedInvalidate = true) override;
        virtual bool SetScrollPos(SIZE szPos, bool bMsg = true, bool seeking = false) override;
        virtual void DoScroll(int x, int y);

        void ProcessScrollBar(SIZE szAvailable, int cxRequired, int cyRequired);

        bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

        void SetAdapter(ListViewAdapter* adapter);
        // 设置列表头（多列分隔）
        bool SetHeaderView(CControlUI* view);
        // 获取列表头（多列分隔）
        CControlUI* GetHeader() const { return _headerView; };
        // 应用列表头（多列分隔）
        void ApplyMultiColumn(CControlUI* pControl, const RECT & rcItem);

        // 添加类表头；添加普通视图时，需要『普通视图适配器』，若无适配器则自动建立，若不是旧版适配器则添加失败。
        bool Add(CControlUI* pControl) override;
        // see ListView::Add，添加类表头时，忽略 |iIndex|
        bool AddAt(CControlUI* pControl, int iIndex) override;

        void SetReferenceItemView(CControlUI* view)
        {  
            m_HiddenItem = view;
        }; 

        CStdPtrArray & GetRecyclePool() {  
            return _recyclePool;
        };
        void DoEvent(TEventUI& event);
        //void NeedUpdate();
    protected:
        ListViewAdapter* _adapter;
        CControlUI* m_HiddenItem;          // 用于计算每个列表项的尺寸
        bool _bHasBasicViewAdapter;        // 是否具有『普通视图适配器』，每个位置分别建立视图。
        // 列表头
        CListHeaderUI* _headerView; 
        bool _headerViewInit=false;
        static CControlUI* _dummyItem;
        bool m_bScrollProcess;                  //  是否需要强制刷新数据
        bool m_data_updated;                  //  是否需要强制刷新数据
        std::map<CControlUI*, int> m_itemview_index_map;    // 缓存每个view所绑定的项目序号

        int m_first_visible_index;               //  第一个可见view对应的index
        int m_first_itemview_top_offset;    //  第一个可见view的top偏移量

        int m_line_height;          // 滚动一行时所滚动的高度
        int m_total_height;         // 整个列表需要占用的高度
        int m_available_height;  // 列表的可见部分高度
        int m_available_width;  // 列表的可见部分高度

        int _scrollYProxy;
        int _lastScrollPositionY;
        int _scrollPositionY;
        int _scrollOffsetY;
        bool _heteroHeight;

        CStdPtrArray _recyclePool;
        int _avgHeight;
        TListInfoUI m_ListInfo;
    };

}


#endif