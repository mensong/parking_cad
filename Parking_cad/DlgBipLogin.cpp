// DlgBipLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgBipLogin.h"
#include "afxdialogex.h"
#include "ModulesManager.h"
#include "lib_bipsignin.h"
#include "Convertor.h"
#include "DBHelper.h"
#include "DES_CBC_5\DesHelper.h"
#include <shlwapi.h>

#define CONFIG_FILE "bip.ini"
#define CONFIG_ENCRYPT_KEY   "B-G-Y++012345678"
#define CONFIG_ENCRYPT_CHAIN "BBGGYYBBGGYYBBGG"

#define SAVE_PWD_ENCRYPT_KEY "B-G-Y++"
#define SAVE_PWD_FILE "parking_cad_user.ini"

// CDlgBipLogin 对话框

IMPLEMENT_DYNAMIC(CDlgBipLogin, CAcUiDialog)

CDlgBipLogin::CDlgBipLogin(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgBipLogin::IDD, pParent)
	, loginSuccess(false)
{

}

CDlgBipLogin::~CDlgBipLogin()
{
}

std::string CDlgBipLogin::_getSavePwdFilePath()
{
	char szTempDir[MAX_PATH];
	DWORD dwRetVal = GetTempPathA(MAX_PATH, szTempDir);
	if (dwRetVal < MAX_PATH && (dwRetVal > 0))
	{
		std::string sTempFile = szTempDir;
		sTempFile += SAVE_PWD_FILE;
		return sTempFile;
	}

	return "";
}

void CDlgBipLogin::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_CHK_SAVE_PWD, m_chkSavePwd);
}


BOOL CDlgBipLogin::OnInitDialog()
{
	if (CAcUiDialog::OnInitDialog() != TRUE)
		return FALSE;

	loginSuccess = false;

	std::string sTempFile = _getSavePwdFilePath();
	if (sTempFile != "")
	{
		if (::PathFileExistsA(sTempFile.c_str()))
		{
			m_chkSavePwd.SetCheck(TRUE);

			char mm[1024];
			DWORD len = ::GetPrivateProfileStringA("BIP", "user", "", mm, 1024, sTempFile.c_str());
			if (len < 1024 && len >0)
			{
				std::string sUser = GL::DES_cbc_decrypt_base64(mm, strlen(mm), 
					SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY), SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY));
				m_editUser.SetWindowText(GL::Ansi2WideByte(sUser.c_str()).c_str());
			}

			len = ::GetPrivateProfileStringA("BIP", "pwd", "", mm, 1024, sTempFile.c_str());
			if (len < 1024 && len >0)
			{
				std::string sPwd = GL::DES_cbc_decrypt_base64(mm, strlen(mm),
					SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY), SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY));
				m_editPassword.SetWindowText(GL::Ansi2WideByte(sPwd.c_str()).c_str());
			}
		}
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgBipLogin, CAcUiDialog)
	ON_BN_CLICKED(IDOK, &CDlgBipLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgBipLogin::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgBipLogin 消息处理程序


void CDlgBipLogin::OnBnClickedOk()
{
	CString sUser;
	m_editUser.GetWindowText(sUser);
	CString sPassword;
	m_editPassword.GetWindowText(sPassword);

	if (sUser.IsEmpty() || sPassword.IsEmpty())
	{
		AfxMessageBox(_T("请输入用户名和密码。"));
		return;
	}

	std::string aUser = GL::WideByte2Ansi(sUser.GetString());
	std::string aPassword = GL::WideByte2Ansi(sPassword.GetString());

#if 0
	//写配置文件
	typedef void(*FN_WriteConfigFile)(
		const char* loginUrl, const char* getUserInfoUrl,
		const char* clientId, const char* clientSecret,
		const char* aesKey, const char* aesChain,
		const char* configFile, const char* iniEncryptKey/* = NULL*/, const char* iniEncryptChain/* = NULL*/);
	FN_WriteConfigFile WriteConfigFile = ModulesManager::Instance().func<FN_WriteConfigFile>("lib_bipsignin.dll", "WriteConfigFile");
	if (WriteConfigFile)
	{
		WriteConfigFile(
			"https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/authenticate", "https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/getUserInfo",//测试
			//"https://login.countrygarden.com.cn/idp/oauth2/authenticate", "https://login.countrygarden.com.cn/idp/oauth2/getUserInfo",//生产
			//"https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/authenticate","https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/getUserInfo",//私有云
			"bppk", "46e8b3aaf78a4c9e880bbc4677ec33cb", "abcdef0123456789", "9iuj87y2hbi5wxl1",
			(DBHelper::GetArxDirA() + CONFIG_FILE).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);
	}
#endif

	typedef LibBipSignIn* (*FN_CreateSingnIn)(void);
	typedef void(*FN_ReleaseSingnIn)(LibBipSignIn* p);
	FN_CreateSingnIn CreateSingnIn = ModulesManager::Instance().func<FN_CreateSingnIn>("lib_bipsignin.dll", "CreateSingnIn");
	FN_ReleaseSingnIn ReleaseSingnIn = ModulesManager::Instance().func<FN_ReleaseSingnIn>("lib_bipsignin.dll", "ReleaseSingnIn");
	if (CreateSingnIn && ReleaseSingnIn)
	{
		LibBipSignIn* bip = CreateSingnIn();

		int nRet = bip->init((DBHelper::GetArxDirA() + CONFIG_FILE).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);
		if (nRet == 0)
		{
			const char* errMsg = NULL;
			nRet = bip->login(aUser.c_str(), aPassword.c_str(), &errMsg);
			if (nRet != 0)
			{
				std::wstring wErrMsg = GL::Ansi2WideByte(errMsg);
				CString sErrMsg;
				sErrMsg.Format(_T("登录失败：%s"), wErrMsg.c_str());
				AfxMessageBox(sErrMsg);
			}
			UserInfo ui;
			nRet = bip->getUserInfo(ui);
			if (nRet == 0)
			{
				loginSuccess = true;
				std::wstring wName = GL::Ansi2WideByte(ui.displayName);
				userName = wName.c_str();
				CString sName;
				sName.Format(_T("登录成功，用户名：%s"), wName.c_str());
				AfxMessageBox(sName);
			}
			bip->uninit();
		}
		else
		{
			CString sErrMsg;
			sErrMsg.Format(_T("统一登录认证配置有误，请重新安装程序：%d"), nRet);
			AfxMessageBox(sErrMsg);
		}

		ReleaseSingnIn(bip);
	}

	if (!loginSuccess)
		return;

	_savePwd();

	CAcUiDialog::OnOK();
}

void CDlgBipLogin::_savePwd()
{
	std::string sTempFile = _getSavePwdFilePath();
	if (sTempFile != "")
	{
		if (m_chkSavePwd.GetCheck() == TRUE)
		{
			CString sUser;
			m_editUser.GetWindowText(sUser);
			CString sPassword;
			m_editPassword.GetWindowText(sPassword);
			if (sUser.IsEmpty() || sPassword.IsEmpty())
			{
				return;
			}
			std::string aUser = GL::WideByte2Ansi(sUser.GetString());
			std::string aPassword = GL::WideByte2Ansi(sPassword.GetString());

			std::string mmUser = GL::DES_cbc_encrypt_base64(aUser.c_str(), strlen(aUser.c_str()),
				SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY), SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY));
			std::string mmPassword = GL::DES_cbc_encrypt_base64(aPassword.c_str(), strlen(aPassword.c_str()),
				SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY), SAVE_PWD_ENCRYPT_KEY, strlen(SAVE_PWD_ENCRYPT_KEY));

			::WritePrivateProfileStringA("BIP", "user", mmUser.c_str(), sTempFile.c_str());
			::WritePrivateProfileStringA("BIP", "pwd", mmPassword.c_str(), sTempFile.c_str());
		}
		else
		{
			::DeleteFileA(sTempFile.c_str());
		}
	}
}



void CDlgBipLogin::OnBnClickedCancel()
{
	_savePwd();

	CAcUiDialog::OnCancel();
}
