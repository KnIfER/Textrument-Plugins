#pragma once
#include "IVirtualDataProvider.h"
#include "VirListBodyUI.h"
#include <list>



/*class CListUI;*/
class CVirtualListUI : public CListUI
{
public:
	CVirtualListUI();
	~CVirtualListUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event) override;
	
	void SetItemCount(int nCount);

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
	* @brief 删除所有子项
	* @return 无
	*/
	virtual void RemoveAll() override;

	bool Remove(CControlUI* pControl);
	bool RemoveAt(int iIndex);

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
	* @brief 获取选中的索引 (通过IVirtualDataProvider * 获取数据) 
	* @return std::vector<int> 选中索引
	*/
	std::vector<int> GetSelectIndex();
protected:
	/// 重写父类接口，提供个性化功能

	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;

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


private:

	void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);


protected:
	bool SelectItem(int iIndex, bool bTakeFocus = false);
// 	bool SelectItemActivate(int iIndex);    // 双击选中
// 
 	bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
// 	void SetMultiSelect(bool bMultiSel);
// 	bool IsMultiSelect() const;
// 	bool UnSelectItem(int iIndex, bool bOthers = false);
 	void SelectAllItems();
 	void UnSelectAllItems();
// 	//支持范围选择 add by ll  2019.04.02
 	virtual bool SelectRange(int iIndex, bool bTakeFocus = false);

protected:
	bool m_bIsStartBoxSel = false;//是否开始框选

	int m_iStartBoxIndex = -1; //开始框选索引
	int m_iEndBoxIndex = -1;//结束框选索引
	POINT m_BoxStartPt;//框选开始起点


	IVirtualDataProvider*  m_pDataProvider ;//OwnerData

	CVirListBodyUI* m_pList;

	int m_nOwnerElementHeight;	// 每个项的高度	
	int m_nOwnerItemCount;	// 列表真实控件数量上限  
	int m_nOldYScrollPos;
	bool m_bArrangedOnce;
	bool m_bForceArrange;	// 强制布局标记

	int m_iCurlShowBeginIndex = 0;
	int m_iCurlShowEndIndex = 0;

	bool m_bScrollProcess = false;//防止SetPos 重复调用, 导致死循环
};
