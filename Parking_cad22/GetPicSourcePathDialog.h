#pragma once
#include <afxdialogex.h>


// CGetPicSourcePathDialog 对话框

class CGetPicSourcePathDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGetPicSourcePathDialog)

public:
	CGetPicSourcePathDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGetPicSourcePathDialog();

	void OnClose();
	BOOL OnInitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAPSIGNPATH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString mPicSourcePath;
};
