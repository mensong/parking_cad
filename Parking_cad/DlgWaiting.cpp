// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
//#include <atomic>
#include "DBHelper.h"
#include "DlgAiParking.h"
#include "Convertor.h"
#include <json/json.h>
#include "ModulesManager.h"
#include <process.h>
#include "GeHelper.h"
#include "OperaAddFrame.h"
#include <iosfwd>
#include <string>
#include "EquipmentroomTool.h"
#include "OperaAxleNetMaking.h"
#include "CommonFuntion.h"
#include "GeHelper.h"
#include "OperaMultiSchemeShow.h"
#include "OperaCheck.h"
#include "LibcurlHttp.h"

std::string CDlgWaiting::ms_uuid;
bool CDlgWaiting::ms_bUseV1;
bool CDlgWaiting::ms_bUseManyShow;
std::string CDlgWaiting::ms_strGeturlPortone;
std::string CDlgWaiting::ms_strGeturlPorttwo;

HANDLE g_hThread = NULL;
CDlgWaiting* g_dlg = NULL;
int g_nShowedCount = 0;

static UINT WINAPI _ThreadAnimation(LPVOID pParam)
{
	g_dlg->DoModal();

	delete g_dlg;
	g_dlg = NULL;
	g_hThread = NULL;
	g_nShowedCount = 0;

	return 0;
}

void CDlgWaiting::Show(bool bShow/* = true*/)
{

	if (g_dlg == NULL && bShow)
	{
		g_dlg = new CDlgWaiting;
		unsigned int nDummy;
		g_hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadAnimation, NULL, 0, &nDummy);

		while (!g_dlg->m_bIsReady)
			Sleep(10);
	}

	if (bShow)
	{
		if (g_nShowedCount < 1)
		{
			g_dlg->ShowWindow(SW_SHOW);
			g_dlg->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		++g_nShowedCount;
	}
	else
	{
		--g_nShowedCount;
		if (g_nShowedCount < 1)
		{
			Destroy();
			g_nShowedCount = 0;
		}
	}
}

void CDlgWaiting::Destroy()
{
	if (g_dlg)
	{
		g_dlg->OnOK();
	}
}

void CDlgWaiting::setUuid(const std::string& uuid, const bool& useV1, const bool& useManyShow)
{
	ms_uuid = uuid;
	ms_bUseV1 = useV1;
	ms_bUseManyShow = useManyShow;
}

void CDlgWaiting::setGetUrlPortOne(const std::string& geturlPortone)
{
	ms_strGeturlPortone = geturlPortone;
}

void CDlgWaiting::setGetUrlPortTwo(const std::string& geturlPorttwo)
{
	ms_strGeturlPorttwo = geturlPorttwo;
}
// CDlgWaiting 对话框

IMPLEMENT_DYNAMIC(CDlgWaiting, CAcUiDialog)

CDlgWaiting::CDlgWaiting(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgWaiting::IDD, pParent)
	, m_bIsReady(false)
{

}

CDlgWaiting::~CDlgWaiting()
{
}

static void __smartLog(bool*& data, bool isDataValid)
{
	if (isDataValid)
	{
		bool end = (*data);
		if (!end)
			CParkingLog::AddLogA("DEBUG_不支持尝试执行的操作", 0, "CDlgWaiting::DoDataExchange");
		delete data;
	}
}

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	Smart<bool*> end(new bool(false), __smartLog);
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_GIF, m_ctrlGif);
	DDX_Control(pDX, IDC_STA_STATUS, m_staStatusText);
	*(end()) = true;
}


BOOL CDlgWaiting::OnInitDialog()
{
	BOOL bRet = CAcUiDialog::OnInitDialog();

	DlgHelper::AdjustPosition(this, DlgHelper::CENTER);

	CString strFile = GetUserDir() + _T("\\loading.gif");
	if (m_ctrlGif.Load(strFile))
	{
		m_ctrlGif.Draw();
	}

	m_bIsReady = true;

	SetTimer(1, 3000, NULL);

	return bRet;
}

BOOL CDlgWaiting::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CAcUiDialog::PreTranslateMessage(pMsg);
}

void CDlgWaiting::OnOK()
{
	CAcUiDialog::OnOK();
	delete this;
}

void CDlgWaiting::OnCancel()
{
	//CAcUiDialog::OnCancel();
}

BEGIN_MESSAGE_MAP(CDlgWaiting, CAcUiDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgWaiting 消息处理程序


void CDlgWaiting::OnTimer(UINT_PTR nIDEvent)
{
	std::string json;
	if (nIDEvent == 1)
	{
		KillTimer(nIDEvent);

		//调用检查是否计算完成函数（）；
		std::string sMsg;
		CString sIndex;
		int iCount = 0;
		int status;
		if (ms_bUseManyShow)
		{
			status = getJsonForLocal(json, sMsg, sIndex, iCount);
		}
		else
		{
			status = getStatus(json, sMsg, sIndex);
		}

		//如果完成后
		if (status == 2)
		{
			//CDlgWaiting::Show(false);
			this->OnOK();
			CString sMsg;
			if (ms_bUseManyShow)
			{
#if 0	//是否去掉调试信息。1:调试；0:发版
				std::string sConfigFile = DBHelper::GetArxDirA() + "rr.json";
				FileHelper::WriteFile(sConfigFile.c_str(), json.c_str(), json.size(), NULL, 0);
#endif
				COperaMultiSchemeShow::getJsonData(json);
				DBHelper::CallCADCommand(_T("MultiSchemeShow "));
				acutPrintf(_T("\nthe uuid is :%s"), GL::Ansi2WideByte(ms_uuid.c_str()).c_str());
				acutPrintf(_T("\n"));
				DBHelper::CallCADCommandEx(_T("Redraw"));
			}
			COperaCheck::setUuid(ms_uuid);

			//CEquipmentroomTool::pritfCurTime();
		}
		else if (status == 0)
		{
			m_sStatus = _T("任务正在排队中，当前排在第") + sIndex + _T("位。");
			m_staStatusText.SetWindowText(m_sStatus);

			SetTimer(nIDEvent, 3000, NULL);
		}
		else if (status == 1)
		{
			m_sStatus = _T("任务正在进行中……");
			m_staStatusText.SetWindowText(m_sStatus);

			SetTimer(nIDEvent, 3000, NULL);
		}
		else
		{
			KillTimer(nIDEvent);
			//CDlgWaiting::Show(false);
			this->OnOK();
			acedAlert(GL::Ansi2WideByte(sMsg.c_str()).c_str());
		}

		return;
	}

	CAcUiDialog::OnTimer(nIDEvent);
}

int CDlgWaiting::getStatus(std::string& json, std::string& sMsg, CString& sIndex)
{
	if (ms_uuid == "")
	{
		sMsg = "uuid不能为空。";
		return 4;
	}

	//std::string httpUrl = "http://10.8.212.187/query/";
	std::string tempUrl;
	if (ms_bUseV1)
	{
		tempUrl = ms_strGeturlPortone + ms_uuid;
	}
	else
	{
		tempUrl = ms_strGeturlPorttwo + ms_uuid;
	}

	const char * sendUrl = tempUrl.c_str();

	HTTP_CLIENT::Ins().setTimeout(60);
	int code = HTTP_CLIENT::Ins().get(sendUrl);
	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = tempUrl + ":网络或服务器错误。(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	int len = 0;
	json = HTTP_CLIENT::Ins().getBody(len);

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
		//WriteFile("result.json", json.c_str(), json.size(), NULL, 0, false);

		int status = root["status"].asInt();
		int index = root["index"].asInt();
		sIndex.Format(_T("%d"), index);
		sMsg = GL::Utf82Ansi(root["message"].asString().c_str()).c_str();
		return status;
	}

	sMsg = "结果格式有误。";
	return 3;
}

void CDlgWaiting::setLayerClose(const CString& layerName)
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return;
	}
	if (pLayerTbl->has(layerName))
	{
		AcDbLayerTableRecord *pLTR;
		pLayerTbl->getAt(layerName, pLTR, AcDb::kForWrite);
		DBHelper::SetLayerIsOff(pLTR);
		pLTR->close();
	}
	pLayerTbl->close();
}

int CDlgWaiting::getJsonForLocal(std::string& json, std::string& sMsg, CString& sIndex, int& iCount)
{
	if (ms_uuid == "")
	{
		sMsg = "uuid不能为空。";
		return 4;
	}

	std::string tempUrl;
	if (ms_bUseV1)
	{
		tempUrl = ms_strGeturlPortone + ms_uuid;
	}
	else
	{
		tempUrl = ms_strGeturlPorttwo + ms_uuid;
	}

	const char * sendUrl = tempUrl.c_str();

	//MessageBoxA(NULL, sendUrl, "url", MB_OK);

	HTTP_CLIENT::Ins().setTimeout(60);
	int code = HTTP_CLIENT::Ins().get(sendUrl);

	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = tempUrl + ":网络或服务器错误。(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	int len = 0;
	json = HTTP_CLIENT::Ins().getBody(len);

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
#ifdef _DEBUG
		WriteFile("result.json", json.c_str(), json.size(), NULL, 0, false);
#endif
		int status = root["status"].asInt();
		int index = root["index"].asInt();
		iCount = root["count"].asInt();
		sIndex.Format(_T("%d"), index);
		sMsg = GL::Utf82Ansi(root["message"].asString().c_str()).c_str();
		return status;
	}

	sMsg = "结果格式有误。";
	return 3;
}

