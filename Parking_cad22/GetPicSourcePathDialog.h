#pragma once
#include <afxdialogex.h>


// CGetPicSourcePathDialog �Ի���

class CGetPicSourcePathDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGetPicSourcePathDialog)

public:
	CGetPicSourcePathDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGetPicSourcePathDialog();

	void OnClose();
	BOOL OnInitDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAPSIGNPATH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString mPicSourcePath;
};
