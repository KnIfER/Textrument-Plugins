#pragma once
#include "IVirtualDataProvider.h"

class CVirListBodyUI : public CListBodyUI
{
public:
	CVirListBodyUI(CListUI* pOwner);
	~CVirListBodyUI();

	void SetVerScrollRange(int nRange);

	void SetItemCount(int nCount);

	//强制重绘,用于外部改变某些项内容
	void RedrawItems(int iItemFrom, int iItemTo);

	//void DoEvent(TEventUI& event);

	/**
	* @brief 设置数据代理对象
	* @param[in] pProvider 开发者需要重写 IVirtualDataProvider 的接口来作为数据代理对象
	* @return 无
	*/
	virtual void SetDataProvider(IVirtualDataProvider * pDataProvider);

	virtual IVirtualDataProvider * GetDataProvider();

	/**
	* @brief 设置子项高度
	* @param[in] nHeight 高度值
	* @return 无
	*/
	virtual void SetElementHeight(int nHeight);

	/**
	* @brief 初始化子项
	* @param[in] nMaxItemCount 初始化数量，默认 50
	* @return 无
	*/
	virtual void InitElement(int nMaxItemCount = 50);

	/**
	* @brief 刷新列表
	* @return 无
	*/
	virtual void Refresh();

	/**
	* @brief 删除所有子项
	* @return 无
	*/
	virtual void RemoveAll() override;

	/**
	* @brief 设置是否强制重新布局
	* @param[in] bForce 设置为 true 为强制，否则为不强制
	* @return 无
	*/
	void SetForceArrange(bool bForce);

	/**
	* @brief 获取当前所有可见控件的索引
	* @param[out] collection 索引列表
	* @return 无
	*/
	void GetDisplayCollection(std::vector<int>& collection);

	/**
	* @brief 让控件在可见范围内
	* @param[in] iIndex 控件索引
	* @param[in] bToTop 是否在最上方
	* @return 无
	*/
	void EnsureVisible(int iIndex, bool bToTop = false);

	/**
	* @brief 设置位置(子控件以及本身)
	* @param[in] rc 本身控件位置 
	* @param[in] bNeedInvalidate 是否刷新
	* @return 无
	*/
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true);

	/// 重写父类接口，提供个性化功能
	/**
	* @brief 滚动条改变,设置滚动条位置,做是否刷新界面
	* @param[in] szPos 滚动条位置
	* @return 无
	*/
	virtual void SetScrollPos(SIZE szPos, bool bMsg = true);

	void EnsureVisible(int iIndex);

	/**
	* @brief 重新布局子项
	* @param[in] bForce 是否强制重新布局
	* @return 无
	*/
	void ReArrangeChild(bool bForce);
protected:

	/**
	* @brief 设置实际需要的滚动条位置
	* @param[in] rc 控件位置
	* @return 无
	*/
	void SetPosInternally(RECT rc);

	/**
	* @brief 添加一个子项
	* @param[in] iIndex 要插入的位置
	* @return 无
	*/
	void AddElement(int iIndex);//对于虚拟列表好像没什么用

	/**
	* @brief 移除一个子项
	* @param[in] iIndex 要移除的子项索引
	* @return 无
	*/
	void RemoveElement(int iIndex);

	/**
	* @brief 键盘按下通知
	* @param[in] ch 按键
	* @return 无
	*/
	virtual void OnKeyDown(TCHAR ch) {}

	/**
	* @brief 键盘弹起通知
	* @param[in] ch 按键
	* @return 无
	*/
	virtual void OnKeyUp(TCHAR ch) {}

private:
	enum ScrollDirection
	{
		kScrollUp = -1,
		kScrollDown = 1
	};

	/**
	* @brief 创建一个子项
	* @return 返回创建后的子项指针
	*/
	CControlUI* CreateElement();

	/**
	* @brief 填充指定子项
	* @param[in] control 子项控件指针
	* @param[in] index 索引
	* @return 返回创建后的子项指针
	*/
	void FillElement(CControlUI *pControl, int iIndex);

	/**
	* @brief 获取元素总数
	* @return 返回元素总指数
	*/
	int GetElementCount();

	/**
	* @brief 使用默认布局
	* @return 成功返回 true，否则返回 false
	*/
	bool UseDefaultLayout();

	/**
	* @brief 得到n个元素对应的高度和，
	* @param[in] nCount 要得到多少元素的高度，-1表示全部元素
	* @return 返回指定数量元素的高度和
	*/
	int CalcElementsHeight(int nCount);

	/**
	* @brief 得到可见范围内第一个元素的前一个元素索引
	* @param[out] bottom 返回上一个元素的 bottom 值
	* @return 返回上一个元素的索引
	*/
	int GetTopElementIndex(int &bottom);

	/**
	* @brief 判断某个元素是否在可见范围内
	* @param[in] iIndex 元素索引
	* @return 返回 true 表示可见，否则为不可见
	*/
	bool IsElementDisplay(int iIndex);

	/**
	* @brief 判断是否要重新布局
	* @param[out] direction 向上滚动还是向下滚动
	* @return true 为需要重新布局，否则为 false
	*/
	bool NeedReArrange(ScrollDirection &direction);

	//未使用
	void Arrange();

public:
	/**
	* @brief 每次重新刷新加载时 重新按照m_vSelIndex 中的索引初始化
	* @param[in] rc 位置
	* @param[in] cxRequired 水平需要的滚动条 Range 
	* @param[in] cyRequired 垂直需要的滚动条Range
	* @return无
	*/
	void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

	/**
	* @brief 每次重新刷新加载时 重新按照m_vSelIndex 中的索引初始化
	* @return无
	*/
	void InitSelectItem();


	/**
	* @brief 单选
	* @param[in] iIndex 选中索引
	* @param[in] bTakeFocus 是否获取焦点
	* @return true 是否选中
	*/
	bool SelectItem(int iIndex, bool bTakeFocus = false);

	/**
	* @brief 多选
	* @param[in] iIndex 选中索引
	* @param[in] bTakeFocus 是否获取焦点
	* @return true 是否选中
	*/
 	bool SelectMultiItem(int iIndex, bool bTakeFocus = false);

/// 不需要重写 begin
// 	bool SelectItemActivate(int iIndex);    // 双击选中
// 	void SetMultiSelect(bool bMultiSel); 
// 	bool IsMultiSelect() const;
//	bool UnSelectItem(int iIndex, bool bOthers = false);

/// 不需要重写 end
	/**
	* @brief 全选
	* @return无
	*/
 	void SelectAllItems();

	/**
	* @brief 清空所有选中
	* @return 无
	*/
 	void UnSelectAllItems();
 	//支持范围选择 add by ll  2019.04.02
	/**
	* @brief 范围选中  m_iSelIndex ~ iIndex 区域选中
	* @param[in] iIndex 选中结束索引
	* @param[in] bTakeFocus 是否获取焦点
	* @return true 是否选中
	*/
 	virtual bool SelectRange(int iIndex, bool bTakeFocus = false);

	/**
	* @brief 获取选中的索引 (通过IVirtualDataProvider * 获取数据)
	* @return std::vector<int> 选中索引
	*/
	std::vector<int> GetSelectIndex();
private:
	bool m_bIsStartBoxSel = false;//是否开始框选

	int m_iStartBoxIndex = -1; //开始框选索引
	int m_iEndBoxIndex = -1;//结束框选索引
	POINT m_BoxStartPt = { 0,0 };//框选开始起点

					   //OwnerData
	IVirtualDataProvider*  m_pDataProvider;

	int m_nOwnerElementHeight = 0;	// 每个项的高度	
	int m_nOwnerItemCount = 0;	// 列表真实控件数量上限  
	int m_nOldYScrollPos = 0;
	bool m_bArrangedOnce = false;
	bool m_bForceArrange = false;	// 强制布局标记

	int m_iCurlShowBeginIndex = 0;
	int m_iCurlShowEndIndex = 0;

	bool m_bScrollProcess = false;//防止SetPos 重复调用, 导致死循环

	int m_iSelIndex = -1;
	std::vector<int> m_vSelIndex;//选中索引
};