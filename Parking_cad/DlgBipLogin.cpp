// DlgBipLogin.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgBipLogin.h"
#include "afxdialogex.h"
#include "ModulesManager.h"
#include "lib_bipsignin.h"
#include "LibcurlHttp.h"
#include "Convertor.h"
#include "DBHelper.h"
#include "DES_CBC_5\DesHelper.h"
#include <shlwapi.h>
#include "json/json.h"
#include "MyMessageBox.h"
#include "KV.h"

#define CONFIG_FILE_DEV "bip_dev.ini"
#define CONFIG_FILE "bip.ini"
#define CONFIG_ENCRYPT_KEY   "B-G-Y++012345678"
#define CONFIG_ENCRYPT_CHAIN "BBGGYYBBGGYYBBGG"

#define SAVE_PWD_ENCRYPT_KEY "B-G-Y++"
#define SAVE_PWD_FILE "parking_cad_user.ini"

// CDlgBipLogin �Ի���

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
			CParkingLog::AddLogA("DEBUG_��֧�ֳ���ִ�еĲ���", 0, "CDlgBipLogin::DoDataExchange");
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

	DlgHelper::AdjustPosition(this, DlgHelper::CENTER);

#if 0
	BIP_SIGNIN::Ins().WriteConfigFile(
		"https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/authenticate", "https://uatlogin.countrygarden.com.cn:8443/idp/oauth2/getUserInfo",//����
		"bppk", "46e8b3aaf78a4c9e880bbc4677ec33cb", "abcdef0123456789", "9iuj87y2hbi5wxl1",
		(DBHelper::GetArxDirA() + CONFIG_FILE_DEV).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);

	BIP_SIGNIN::Ins().WriteConfigFile(
		"https://login.countrygarden.com.cn/idp/oauth2/authenticate", "https://login.countrygarden.com.cn/idp/oauth2/getUserInfo",//����
		"bppk", "f4021359b0ea4f11a20e52f9add990de", "abcdef0123456789", "9iuj87y2hbi5wxl1",
		(DBHelper::GetArxDirA() + CONFIG_FILE).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);
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
			KV::Ins().SetStrA("add_log_url", mm);
	}
	if (m_get_userUrl.empty() || m_add_userUrl.empty())
	{
		::MessageBox(NULL, _T("ȱ��4u7h,ini�����ļ���"), _T("ȱ���ļ�"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgBipLogin, CAcUiDialog)
	ON_BN_CLICKED(IDOK, &CDlgBipLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgBipLogin::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgBipLogin ��Ϣ�������

void CDlgBipLogin::OnBnClickedOk()
{
	CString sBip;
	m_editUser.GetWindowText(sBip);
	CString sPassword;
	m_editPassword.GetWindowText(sPassword);

	if (sBip.IsEmpty() || sPassword.IsEmpty())
	{
		AfxMessageBox(_T("�������û��������롣"));
		return;
	}

	std::string aUser = GL::WideByte2Ansi(sBip.GetString());
	std::string aPassword = GL::WideByte2Ansi(sPassword.GetString());

	LibBipSignIn* bip = BIP_SIGNIN::Ins().CreateSingnIn();

	int nRet = bip->init((DBHelper::GetArxDirA() + CONFIG_FILE).c_str(), CONFIG_ENCRYPT_KEY, CONFIG_ENCRYPT_CHAIN);
	if (nRet == 0)
	{
		const char* errMsg = NULL;
		nRet = bip->login(aUser.c_str(), aPassword.c_str(), &errMsg);
		if (nRet != 0)
		{
			std::wstring wErrMsg = GL::Ansi2WideByte(errMsg);
			CString sErrMsg;
			sErrMsg.Format(_T("ͳһ�����֤ʧ��(%d)��%s"), nRet, wErrMsg.c_str());
			CParkingLog::AddLog(LOG_BIP_LOGIN, 1, sErrMsg, 1, sBip);
			AfxMessageBox(sErrMsg);
		}
		else
		{
			CParkingLog::AddLog(LOG_BIP_LOGIN, 0, _T("BIPͳһ�����֤�ɹ�"), 1, sBip);
		}

		UserInfo ui;
		nRet = bip->getUserInfo(ui);
		if (nRet == 0)
		{
			int code = HTTP_CLIENT::Ins().get_a(m_get_userUrl.c_str(), true, "udid", aUser.c_str(), NULL);

			if (code == 200)
			{
				int nLen = 0;
				const char* pBody = HTTP_CLIENT::Ins().getBody(nLen);
				Json::Value js;
				Json::Reader jsReader;
				if (jsReader.parse(pBody, js))
				{
					std::string jname = js["name"].asString();
					std::string jgroup_udid = js["group_udid"].asString();
					int jallow = js["allow"].asInt();
					std::string jdescr = js["descr"].asString();
					std::string jreg_time = js["reg_time"].asString();
					std::string jlast_signin_time = js["last_signin_time"].asString();
					int jsignin_count = js["signin_count"].asInt();

					std::wstring wName = GL::Utf82WideByte(jname.c_str());
					if (jallow == 1)
					{
						userName = wName.c_str();
						bipId = sBip;
						groupUdid = GL::Utf82WideByte(jgroup_udid.c_str()).c_str();
						allow = jallow;
						descr = GL::Utf82WideByte(jdescr.c_str()).c_str();
						regTime = GL::Utf82WideByte(jreg_time.c_str()).c_str();
						lastSigninTime = GL::Utf82WideByte(jlast_signin_time.c_str()).c_str();
						signinCount = jsignin_count;

						CString sMsg;
						sMsg.Format(_T("��¼�ɹ����û�����%s"), wName.c_str());

						CParkingLog::AddLog(LOG_AUTH_LOGIN, 0, sMsg, 1, sBip);

						loginSuccess = true;
					}
					else
					{
						AfxMessageBox(_T("δ��ͨ�û�Ȩ�ޣ�����ϵ����Ա��"));
					}
				}					
			}
			else if (code == 404)
			{
				std::string aUserName = ui.displayName;

				CString sMsg;
				sMsg.Format(_T("��Ȩʧ�ܣ�bip��%s�����Ƿ����뿪ͨȨ�ޣ�"), sBip);
				std::map<int, CString> btnText;
				btnText[IDOK] = _T("���뿪ͨȨ��");
				btnText[IDCANCEL] = _T("ȡ��");
				int nRes = MyMessageBox::Show(NULL, sMsg, _T("�Ƿ�ͨȨ��"), MB_OKCANCEL, btnText);
				if (nRes == IDOK)
				{
					Json::Value js;
					js["udid"] = aUser;
					js["name"] = GL::Ansi2Utf8(aUserName.c_str());
					js["group_udid"] = "designer";
					js["allow"] = 0;
					js["descr"] = GL::WideByte2Utf8(L"���Կͻ���������û�");
					Json::FastWriter jsWriter;
					std::string sJson = jsWriter.write(js);
					int code = HTTP_CLIENT::Ins().post(m_add_userUrl.c_str(), sJson.c_str(), sJson.size(), true, "application/json");
					if (code == 200)
					{
						AfxMessageBox(_T("����ɹ�����ȴ���ˡ�"));
					}
					else
					{
						AfxMessageBox(_T("����ʧ�ܡ�"));
					}
				}
			}
			else
			{
				::MessageBox(NULL, _T("��������"), _T("����"), MB_OK | MB_ICONERROR);
			}
		}
		bip->uninit();
	}
	else
	{
		CString sErrMsg;
		sErrMsg.Format(_T("ͳһ��¼��֤�������������°�װ����%d"), nRet);
		CParkingLog::AddLog(LOG_BIP_LOGIN, 1, sErrMsg, 1, sBip);
		AfxMessageBox(sErrMsg);
	}

	BIP_SIGNIN::Ins().ReleaseSingnIn(bip);

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

			CParkingLog::AddLog(LOG_SAVE_PASSWORD, 0, _T("�����˺�����"), 1, sUser);
		}
		else
		{
			::DeleteFileA(sTempFile.c_str());
			CParkingLog::AddLog(LOG_SAVE_PASSWORD, 0, _T("�������˺�����"), 1, sUser);
		}
	}
}



void CDlgBipLogin::OnBnClickedCancel()
{
	_savePwd();

	CAcUiDialog::OnCancel();
}
