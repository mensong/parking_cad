/** 
 * @file   	ListCtrlCustom.h
 * @brief  	带下拉框的列表控件
 * ------------------------------------------------------------
 * 版本历史       注释                日期
 * ------------------------------------------------------------
 * @version v1.0  初始版本            7/8/2014
 * ------------------------------------------------------------
 * Note:    此控件只能使用Report样式
 */
			
#pragma once
#include <set>

class CListCtrlCustom;

//控件调整回调函数
//返回TRUE 表示已经做了调整处理，否则表示根据rectGrid并采用默认的处理方法
typedef BOOL (CALLBACK *LPFUNC_SETTING_RECT)(CListCtrlCustom *pListCtrl, CWnd *pCtrl, CRect &rcGrid);

/** 
 * @class  	SDCtrlComboboxListctrl
 * @brief  	带下拉框的列表控件
 * ------------------------------------------------------------
 * 版本历史       注释                日期
 * ------------------------------------------------------------
 * @version v1.0  初始版本            7/8/2014
 * ------------------------------------------------------------
 * Note:   	
 */	
class CListCtrlCustom :
	public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlCustom)

public:
	CListCtrlCustom(void);
	virtual ~CListCtrlCustom(void);

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

public:
	/** 
	 * @brief     	获取某行某列的附加控件
	 * @param       nRow - 行号
	 * @param     	nCol - 列号
	 * @retval    	NULL:此行列没有附加控件
	 * @note      	
	 */
	CWnd *GetCtrl(int nRow, int nCol);

	//获得某行某列的文本
	CString GetText(int nRow, int nCol);
	/** 
	 * @brief     	设置格子内容
	 * @param       nItem - 行
	 * @param     	nSubItem - 列
	 * @param     	pExCtrl - 附加控件
	 * @param		bDelIfExist - 如果原来的格子有控件，是否自动删除
	 * @param		bDelOnListDestory - 是否自动删除
	 * @retval    	TRUE:成功；FALSE: 失败
	 * @note      	
	 */
	BOOL SetItemExCtrl(int nItem, int nSubItem, CWnd *pExCtrl, bool bDelIfExist=true, 
		bool bDelOnListDestory=true, bool bRefresh = true);

	/**
	* @brief     	设置格子的可见性
	* @param        nItem - 行
	* @param     	nSubItem - 列
	* @param     	bVisible - 是否见
	* @retval    	TRUE:成功；FALSE: 失败
	* @note
	*/
	BOOL SetItemExCtrlVisible(int nItem, int nSubItem, bool bVisible, bool bRefresh = true);

	//设置行高
	BOOL SetRowHeight(int nHeight);

	//设置是否自动删除子控件
	void SetAutoReleaseSubCtrls(bool bAutoRelease);

	// Removes a single item from the control.
	BOOL DeleteItem(_In_ int nItem, bool bDelChildCtrl=true);

	// Removes all items from the control.
	BOOL DeleteAllItems(bool bDelChildCtrl=true);

	//设置某列是否可调整列宽
	void SetColumnEnableTrack(int nCol, bool bCanBeTrack);

public:
	//设置附加控件调整回调函数
	void RegResizeEvent(LPFUNC_SETTING_RECT pfnResizeExCtrl);

	//获得当前鼠标的item信息
	int CurPosHitTest(LVHITTESTINFO &ht);

	//获得listctrl格子的范围
	//为什么不直接使用GetSubItemRect呢，那是因为GetSubItemRect对于
	//第一列获取的时候，获取到的宽度会是整行的宽度，所以这里需要修正
	BOOL GetItemRect(int nRow, int nCol, CRect &rect);

	//调整自定义控件的位置
	void UpdateExCtrlsPos();

	//清除所有自定义控件
	void ClearExCtrls();
	
protected:
	inline CString makeRowColKey(int nItem, int nSubItem);

protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);//无论横还是竖的滚动，都会进这个函数
	DECLARE_MESSAGE_MAP()

protected:
	//附加控件列表
	typedef CArray<CWnd *> COLUMN_INFO;
	typedef COLUMN_INFO *LP_COLUMN_INFO;
	typedef CArray<LP_COLUMN_INFO> ROW_EX_CTRL_INFO;
	ROW_EX_CTRL_INFO m_arrExCtrls;

	//<row|col, visible>
	//  true或没有找到 = 可见；false = 不可见
	std::map<CString, bool> m_mpVisible;
	
	//调整控件回调函数
	LPFUNC_SETTING_RECT m_pfnResizeExCtrl;

	//用于设置行高的图标列表
	CImageList m_lstImgForRowHeight;

	//是否自动释放格子控件
	bool m_bAutoReleaseSubCtrls;

	//自动删除的子控件列表
	std::set<CWnd*> m_setAutoDelCtrls;

	//字体，目前只有在设置的高度少于当前字体的高度时使用了这个字体变量
	CFont m_Font;

	std::set<int> m_setNotTrackColumn;//<不能被调整宽度的列号>

private://temp
	std::set<CWnd*> m_setTempCtrlWillDel;

};

//设置主题样式
// 如果不设置，则在 HDN_ENDTRACK 事件中使用GetSubItemRect获得的是旧的尺寸
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif