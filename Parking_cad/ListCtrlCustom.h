/** 
 * @file   	ListCtrlCustom.h
 * @brief  	����������б�ؼ�
 * ------------------------------------------------------------
 * �汾��ʷ       ע��                ����
 * ------------------------------------------------------------
 * @version v1.0  ��ʼ�汾            7/8/2014
 * ------------------------------------------------------------
 * Note:    �˿ؼ�ֻ��ʹ��Report��ʽ
 */
			
#pragma once
#include <set>

class CListCtrlCustom;

//�ؼ������ص�����
//����TRUE ��ʾ�Ѿ����˵������������ʾ����rectGrid������Ĭ�ϵĴ�����
typedef BOOL (CALLBACK *LPFUNC_SETTING_RECT)(CListCtrlCustom *pListCtrl, CWnd *pCtrl, CRect &rcGrid);

/** 
 * @class  	SDCtrlComboboxListctrl
 * @brief  	����������б�ؼ�
 * ------------------------------------------------------------
 * �汾��ʷ       ע��                ����
 * ------------------------------------------------------------
 * @version v1.0  ��ʼ�汾            7/8/2014
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
	 * @brief     	��ȡĳ��ĳ�еĸ��ӿؼ�
	 * @param       nRow - �к�
	 * @param     	nCol - �к�
	 * @retval    	NULL:������û�и��ӿؼ�
	 * @note      	
	 */
	CWnd *GetCtrl(int nRow, int nCol);

	//���ĳ��ĳ�е��ı�
	CString GetText(int nRow, int nCol);
	/** 
	 * @brief     	���ø�������
	 * @param       nItem - ��
	 * @param     	nSubItem - ��
	 * @param     	pExCtrl - ���ӿؼ�
	 * @param		bDelIfExist - ���ԭ���ĸ����пؼ����Ƿ��Զ�ɾ��
	 * @param		bDelOnListDestory - �Ƿ��Զ�ɾ��
	 * @retval    	TRUE:�ɹ���FALSE: ʧ��
	 * @note      	
	 */
	BOOL SetItemExCtrl(int nItem, int nSubItem, CWnd *pExCtrl, bool bDelIfExist=true, 
		bool bDelOnListDestory=true, bool bRefresh = true);

	/**
	* @brief     	���ø��ӵĿɼ���
	* @param        nItem - ��
	* @param     	nSubItem - ��
	* @param     	bVisible - �Ƿ��
	* @retval    	TRUE:�ɹ���FALSE: ʧ��
	* @note
	*/
	BOOL SetItemExCtrlVisible(int nItem, int nSubItem, bool bVisible, bool bRefresh = true);

	//�����и�
	BOOL SetRowHeight(int nHeight);

	//�����Ƿ��Զ�ɾ���ӿؼ�
	void SetAutoReleaseSubCtrls(bool bAutoRelease);

	// Removes a single item from the control.
	BOOL DeleteItem(_In_ int nItem, bool bDelChildCtrl=true);

	// Removes all items from the control.
	BOOL DeleteAllItems(bool bDelChildCtrl=true);

	//����ĳ���Ƿ�ɵ����п�
	void SetColumnEnableTrack(int nCol, bool bCanBeTrack);

public:
	//���ø��ӿؼ������ص�����
	void RegResizeEvent(LPFUNC_SETTING_RECT pfnResizeExCtrl);

	//��õ�ǰ����item��Ϣ
	int CurPosHitTest(LVHITTESTINFO &ht);

	//���listctrl���ӵķ�Χ
	//Ϊʲô��ֱ��ʹ��GetSubItemRect�أ�������ΪGetSubItemRect����
	//��һ�л�ȡ��ʱ�򣬻�ȡ���Ŀ�Ȼ������еĿ�ȣ�����������Ҫ����
	BOOL GetItemRect(int nRow, int nCol, CRect &rect);

	//�����Զ���ؼ���λ��
	void UpdateExCtrlsPos();

	//��������Զ���ؼ�
	void ClearExCtrls();
	
protected:
	inline CString makeRowColKey(int nItem, int nSubItem);

protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);//���ۺỹ�����Ĺ�����������������
	DECLARE_MESSAGE_MAP()

protected:
	//���ӿؼ��б�
	typedef CArray<CWnd *> COLUMN_INFO;
	typedef COLUMN_INFO *LP_COLUMN_INFO;
	typedef CArray<LP_COLUMN_INFO> ROW_EX_CTRL_INFO;
	ROW_EX_CTRL_INFO m_arrExCtrls;

	//<row|col, visible>
	//  true��û���ҵ� = �ɼ���false = ���ɼ�
	std::map<CString, bool> m_mpVisible;
	
	//�����ؼ��ص�����
	LPFUNC_SETTING_RECT m_pfnResizeExCtrl;

	//���������иߵ�ͼ���б�
	CImageList m_lstImgForRowHeight;

	//�Ƿ��Զ��ͷŸ��ӿؼ�
	bool m_bAutoReleaseSubCtrls;

	//�Զ�ɾ�����ӿؼ��б�
	std::set<CWnd*> m_setAutoDelCtrls;

	//���壬Ŀǰֻ�������õĸ߶����ڵ�ǰ����ĸ߶�ʱʹ��������������
	CFont m_Font;

	std::set<int> m_setNotTrackColumn;//<���ܱ�������ȵ��к�>

private://temp
	std::set<CWnd*> m_setTempCtrlWillDel;

};

//����������ʽ
// ��������ã����� HDN_ENDTRACK �¼���ʹ��GetSubItemRect��õ��Ǿɵĳߴ�
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif