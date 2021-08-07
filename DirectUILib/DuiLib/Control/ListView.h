#pragma once


namespace DuiLib {
    class ListViewAdapter {
    public:
        virtual size_t GetItemCount() = 0;
        virtual CControlUI* CreateItemView() = 0;
        virtual void OnBindItemView(CControlUI* view, size_t index) = 0;
    };



    class UILIB_API ListView : public CContainerUI {

        DECLARE_DUICONTROL(ListView)

    public:
        ListView();

        ~ListView(){}

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetPos(RECT rc, bool bNeedInvalidate = true) override;
        virtual void SetScrollPos(SIZE szPos, bool bMsg = true) override;

        bool ProcessVisibleItems(int item_view_count);
        void ProcessScrollBar(SIZE szAvailable, int cxRequired, int cyRequired);
        void UpdateSubviews(RECT rc, bool force_update);

        void SetAdapter(ListViewAdapter* adapter);

        void SetReferenceItemView(CControlUI* view) {  
            m_HiddenItem = view;
        };
        void DoEvent(TEventUI& event);
        void NeedUpdate();
    private:
        ListViewAdapter* m_Delegate;
        CControlUI* m_HiddenItem;          // 用于计算每个列表项的尺寸
        bool m_bScrollProcess;                  //  是否需要强制刷新数据
        bool m_data_updated;                  //  是否需要强制刷新数据
        std::map<CControlUI*, int> m_itemview_index_map;    // 缓存每个view所绑定的项目序号

        int m_first_visible_index;               //  第一个可见view对应的index
        int m_first_itemview_top_offset;    //  第一个可见view的top偏移量

        int m_line_height;          // 滚动一行时所滚动的高度
        int m_total_height;         // 整个列表需要占用的高度
        int m_available_height;  // 列表的可见部分高度
        int m_available_width;  // 列表的可见部分高度
        int m_ScrollY;                 // 列表自己维护的垂直方向的滚动
        int m_ScrollX;                 // 列表自己维护的垂直方向的滚动

    };

}
