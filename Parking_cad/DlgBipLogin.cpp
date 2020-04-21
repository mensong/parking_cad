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
#include "json/json.h"
#include "MyMessageBox.h"
#include "KVHelp.h"

#define CONFIG_FILE_DEV "bip_dev.ini"
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

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgBipLogin::DoDataExchange");
		delete data;
	}
}


void CDlgBipLogin::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_CHK_SAVE_PWD, m_chkSavePwd);
	*(end()) = true;
}


BOOL CDlgBipLogin::OnInitDialog()
{
	if (CAcUiDialog::OnInitDialog() != TRUE)
		return FALSE;

	CenterWindow(GetDesktopWindow());//窗口至于屏幕中间

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
			"bppk", "46e8b3aaf78a4c9e880bbc4677ec33cb", "abcdef0123456789", "9iuj87y2hbi5wxl1",
			(DBHelper::GetArxDirA() + CONFIG_FILE_DEV).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);

		WriteConfigFile(
			"https://login.countrygarden.com.cn/idp/oauth2/authenticate", "https://login.countrygarden.com.cn/idp/oauth2/getUserInfo",//生产
			"bppk", "f4021359b0ea4f11a20e52f9add990de", "abcdef0123456789", "9iuj87y2hbi5wxl1",
			(DBHelper::GetArxDirA() + CONFIG_FILE).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);
	}
#endif

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

	std::string sIni4u7h = DBHelper::GetArxDirA() + "4u7h.ini";
	if (::PathFileExistsA(sIni4u7h.c_str()))
	{
		char mm[1024];
		DWORD len = ::GetPrivateProfileStringA("URL", "get_user", "", mm, 1024, sIni4u7h.c_str());
		if (len < 1024 && len >0)
			m_get_userUrl = mm;
		len = ::GetPrivateProfileStringA("URL", "add_user", "", mm, 1024, sIni4u7h.c_str());
		if (len < 1024 && len >0)
			m_add_userUrl = mm;
		len = ::GetPrivateProfileStringA("URL", "add_log", "", mm, 1024, sIni4u7h.c_str());
		if (len < 1024 && len >0)
			KVHelp::setStrA("add_log_url", mm);
	}
	if (m_get_userUrl.empty() || m_add_userUrl.empty())
	{
		::MessageBox(NULL, _T("缺少4u7h,ini配置文件！"), _T("缺少文件"), MB_OK | MB_ICONERROR);
		return FALSE;
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
				sErrMsg.Format(_T("统一身份认证失败(%d)：%s"), nRet, wErrMsg.c_str());
				CParkingLog::AddLog(LOG_BIP_LOGIN, 1, sErrMsg, 1, sUser);
				AfxMessageBox(sErrMsg);
			}
			else
			{
				CParkingLog::AddLog(LOG_BIP_LOGIN, 0, _T("BIP统一身份认证成功"), 1, sUser);
			}

			UserInfo ui;
			nRet = bip->getUserInfo(ui);
			if (nRet == 0)
			{
				typedef int(*FN_get_a)(const char* url, bool dealRedirect, ...);
				FN_get_a get_a = ModulesManager::Instance().func<FN_get_a>("LibcurlHttp.dll", "get_a");
				typedef int(*FN_post)(const char* url, const char*, int, bool, const char*);
				FN_post post = ModulesManager::Instance().func<FN_post>("LibcurlHttp.dll", "post");
				typedef const char* (*FN_getBody)(int& len);
				FN_getBody getBody = ModulesManager::Instance().func<FN_getBody>("LibcurlHttp.dll", "getBody");
				if (!get_a || !post || !getBody)
				{
					CParkingLog::AddLog(LOG_MISS_MOD, 1, _T("缺少LibcurlHttp.dll模块！"), 1, sUser);
					::MessageBox(NULL, AcString(_T("缺少LibcurlHttp.dll模块！")), _T("缺少模块"), MB_OK | MB_ICONERROR);
					return;
				}
				
				int code = get_a(m_get_userUrl.c_str(), true, "udid", aUser.c_str(), NULL);

				if (code == 200)
				{
					int nLen = 0;
					const char* pBody = getBody(nLen);
					Json::Value js;
					Json::Reader jsReader;
					if (jsReader.parse(pBody, js))
					{
						std::string name = js["name"].asString();
						std::string group_udid = js["group_udid"].asString();
						int allow = js["allow"].asInt();
						std::string descr = js["descr"].asString();
						std::string reg_time = js["reg_time"].asString();
						std::string last_signin_time = js["last_signin_time"].asString();
						int signin_count = js["signin_count"].asInt();

						std::wstring wName = GL::Utf82WideByte(name.c_str());
						if (allow == 1)
						{
							userName = wName.c_str();
							bipId = sUser;

							CString sMsg;
							sMsg.Format(_T("登录成功，用户名：%s"), wName.c_str());

							CParkingLog::AddLog(LOG_AUTH_LOGIN, 0, sMsg, 1, sUser);

							loginSuccess = true;
						}
						else
						{
							AfxMessageBox(_T("未开通用户权限，请联系管理员。"));
						}
					}					
				}
				else if (code == 404)
				{
					std::string aUserName = ui.displayName;

					CString sMsg;
					sMsg.Format(_T("鉴权失败（bip：%s），是否申请开通权限？"), sUser);
					std::map<int, CString> btnText;
					btnText[IDOK] = _T("申请开通权限");
					btnText[IDCANCEL] = _T("取消");
					int nRes = MyMessageBox::Show(NULL, sMsg, _T("是否开通权限"), MB_OKCANCEL, btnText);
					if (nRes == IDOK)
					{
						Json::Value js;
						js["udid"] = aUser;
						js["name"] = GL::Ansi2Utf8(aUserName.c_str());
						js["group_udid"] = "designer";
						js["allow"] = 0;
						js["descr"] = GL::WideByte2Utf8(L"来自客户端申请的用户");
						Json::FastWriter jsWriter;
						std::string sJson = jsWriter.write(js);
						int code = post(m_add_userUrl.c_str(), sJson.c_str(), sJson.size(), true, "application/json");
						if (code == 200)
						{
							AfxMessageBox(_T("申请成功，请等待审核。"));
						}
						else
						{
							AfxMessageBox(_T("申请失败。"));
						}
					}
				}
				else
				{
					::MessageBox(NULL, _T("其它错误！"), _T("错误"), MB_OK | MB_ICONERROR);
				}
			}
			bip->uninit();
		}
		else
		{
			CString sErrMsg;
			sErrMsg.Format(_T("统一登录认证配置有误，请重新安装程序：%d"), nRet);
			CParkingLog::AddLog(LOG_BIP_LOGIN, 1, sErrMsg, 1, sUser);
			AfxMessageBox(sErrMsg);
		}

		ReleaseSingnIn(bip);
	}
	else
	{
		CString sErrMsg;
		sErrMsg.Format(_T("lib_bipsignin.dll模块加载失败。"));
		CParkingLog::AddLog(LOG_MISS_MOD, 1, _T("缺少lib_bipsignin.dll模块！"), 1, sUser);
		AfxMessageBox(sErrMsg);
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
		CString sUser;
		m_editUser.GetWindowText(sUser);

		if (m_chkSavePwd.GetCheck() == TRUE)
		{
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

			CParkingLog::AddLog(LOG_SAVE_PASSWORD, 0, _T("保存账号密码"), 1, sUser);
		}
		else
		{
			::DeleteFileA(sTempFile.c_str());
			CParkingLog::AddLog(LOG_SAVE_PASSWORD, 0, _T("不保存账号密码"), 1, sUser);
		}
	}
}



void CDlgBipLogin::OnBnClickedCancel()
{
	_savePwd();

	CAcUiDialog::OnCancel();
}
