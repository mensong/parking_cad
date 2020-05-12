// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
//#include <atomic>
#include "DBHelper.h"
#include "ArxDialog.h"
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
	CAcModuleResourceOverride resOverride;
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

	CenterWindow(GetDesktopWindow());//窗口至于屏幕中间

	AcString strFile = DBHelper::GetArxDir() + _T("\\loading.gif");
	if (m_ctrlGif.Load(strFile.constPtr()))
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
	int code = HTTP_CLIENT::Ins().get(sendUrl, true);
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

void CDlgWaiting::parkingShow(AcDbObjectId& parkingId, const AcGePoint2d& parkingShowPt, const double& parkingShowRotation, const CString& blockName)
{
	CString sParkingsLayer(CEquipmentroomTool::getLayerNameByJson("ordinary_parking").c_str());
	CEquipmentroomTool::creatLayerByjson("ordinary_parking");
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);
	AcGeMatrix3d mat;
	AcGeVector3d vec(0, 0, 1);
	mat.setToRotation(parkingShowRotation, vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(parkingId, blockName, mat);
	CEquipmentroomTool::setEntToLayer(parkingId, sParkingsLayer);
}

AcDbObjectId CDlgWaiting::axisShow(const AcGePoint2dArray& axisPts)
{
	CString sAxisLayer(CEquipmentroomTool::getLayerName("parking_axis").c_str());
	CEquipmentroomTool::creatLayerByjson("parking_axis");
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId axisId;
	DBHelper::AppendToDatabase(axisId,pLine);
	pLine->close();
	CEquipmentroomTool::setEntToLayer(axisId, sAxisLayer);
	return axisId;
}

AcDbObjectId CDlgWaiting::laneShow(const AcGePoint2dArray& lanePts)
{
	CString sLaneLayer(CEquipmentroomTool::getLayerName("lane_center_line_and_driving_direction").c_str());
	CEquipmentroomTool::creatLayerByjson("lane_center_line_and_driving_direction");
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId laneId;
	DBHelper::AppendToDatabase(laneId,pLine);
	pLine->close();
	CEquipmentroomTool::setEntToLayer(laneId, sLaneLayer);
	return laneId;
}

void CDlgWaiting::scopeShow(const AcGePoint2dArray& park_columnPts)
{
	CString sScopeLayer(CEquipmentroomTool::getLayerName("rangeline").c_str());
	CEquipmentroomTool::creatLayerByjson("rangeline");
	AcDbPolyline *pPoly = new AcDbPolyline(park_columnPts.length());
	double width = 0;//线宽
	for (int i = 0; i < park_columnPts.length(); i++)
	{
		pPoly->addVertexAt(0, park_columnPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId scopeId;
	DBHelper::AppendToDatabase(scopeId,pPoly);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(scopeId, sScopeLayer);
}

void CDlgWaiting::pillarShow(const AcGePoint2dArray& onePillarPts)
{
	CString sPillarLayer(CEquipmentroomTool::getLayerName("column").c_str());
	CEquipmentroomTool::creatLayerByjson("column");
	AcDbPolyline *pPoly = new AcDbPolyline(onePillarPts.length());
	double width = 0;//线宽
	for (int i = 0; i < onePillarPts.length(); i++)
	{
		pPoly->addVertexAt(0, onePillarPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId pillarId;
	DBHelper::AppendToDatabase(pillarId,pPoly);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(pillarId, sPillarLayer);
}

void CDlgWaiting::arrowShow(const AcGePoint2dArray& oneArrowPts)
{
	CString sArrowLayer(CEquipmentroomTool::getLayerName("arrow").c_str());
	CEquipmentroomTool::creatLayerByjson("arrow");
	AcDbPolyline *pPoly = new AcDbPolyline(oneArrowPts.length());
	double width = 0;//线宽
	for (int i = 0; i < oneArrowPts.length(); i++)
	{
		pPoly->addVertexAt(0, oneArrowPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	AcDbObjectId arrowId;
	DBHelper::AppendToDatabase(arrowId,pPoly);
	pPoly->close();
	CEquipmentroomTool::setEntToLayer(arrowId, sArrowLayer);
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

void CDlgWaiting::creatNewParking(const double& dParkingLength, const double& dParkingWidth, CString& blockName)
{
	double dUseLength = dParkingLength * 1000;
	double dUseWidth = dParkingWidth * 1000;
	AcGePoint2d squarePt1(0, 0);
	AcGePoint2d squarePt2(0, dUseLength);
	AcGePoint2d squarePt3(dUseWidth, dUseLength);
	AcGePoint2d squarePt4(dUseWidth, 0);
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	double width = 20;//矩形方形线宽
	pPoly->addVertexAt(0, squarePt1, 0, width, width);
	pPoly->addVertexAt(1, squarePt2, 0, width, width);
	pPoly->addVertexAt(2, squarePt3, 0, width, width);
	pPoly->addVertexAt(3, squarePt4, 0, width, width);
	pPoly->setClosed(true);
	AcDbObjectId squareId;
	pPoly->setColorIndex(6);
	DBHelper::AppendToDatabase(squareId, pPoly);
	pPoly->close();
	AcDbObjectIdArray allIds;
	allIds.append(squareId);
	AcGePoint3d centerPt(dUseWidth / 2, dUseLength / 2, 0);
	AcDbObjectId parkingId;
	DBHelper::InsertBlkRef(parkingId, _T("car_1"), centerPt);
	allIds.append(parkingId);
	std::vector<AcDbEntity*> blockEnts;
	for (int i = 0; i < allIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, allIds[i], AcDb::kForWrite);
		//判断自定义实体的类型
		if (pEnt == NULL)
			continue;
		blockEnts.push_back(pEnt);
	}
	if (blockEnts.size() < 1)
		return;
	CString sParkingLength;
	CString sParkingWidth;
	sParkingLength.Format(_T("%.1f"), dParkingLength);
	sParkingWidth.Format(_T("%.1f"), dParkingWidth);
	CString parkingName = _T("parking");
	blockName = parkingName + _T("_") + sParkingLength + _T("_") + sParkingWidth;
	DBHelper::CreateBlock(blockName, blockEnts, centerPt);
	for (int j = 0; j < blockEnts.size(); j++)
	{
		blockEnts[j]->erase();
		blockEnts[j]->close();
	}
}



void CDlgWaiting::setLandDismensions(double m_dDis, const AcDbObjectIdArray& RoadLineIds)
{
	CCommonFuntion::creatLaneGridDimensionsDimStyle(_T("车道轴网尺寸标注样式"));//创建新的标注样式
	CString sLanesDimLayer(CEquipmentroomTool::getLayerName("dimensions_dimensiontext").c_str());
	CEquipmentroomTool::creatLayerByjson("dimensions_dimensiontext");
	if (RoadLineIds.length() == 0)
	{
		acutPrintf(_T("\n没有车道信息，生成车道标注失败！"));
		return;
	}
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es;
	for (int i = 0; i < RoadLineIds.length(); ++i)
	{
		es = acdbOpenObject(pEnt, RoadLineIds[i], AcDb::kForRead);
		if (es != eOk)
			continue;
		if (pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine *pLine = AcDbLine::cast(pEnt);
			AcGePoint3d startpoint;
			pLine->getStartPoint(startpoint);
			AcGePoint3d endpoint;
			pLine->getEndPoint(endpoint);
			AcGePoint3d centerpoint = AcGePoint3d((startpoint.x + endpoint.x) / 2, (startpoint.y + endpoint.y) / 2, (startpoint.z + endpoint.z) / 2);

			AcGeVector3d movevec = AcGeVector3d(startpoint - endpoint);
			movevec.normalize();
			centerpoint.transformBy(movevec * 800);

			AcGeVector3d tempVec = AcGeVector3d(startpoint - endpoint);
			AcGeVector3d vec = tempVec.rotateBy(ARX_PI / 2, AcGeVector3d(0, 0, 1));
			vec.normalize();

			AcGePoint3d tempPoint = centerpoint;
			AcGePoint3d movePt1 = tempPoint.transformBy(vec * (m_dDis / 2));
			tempPoint = centerpoint;
			AcGePoint3d movePt2 = tempPoint.transformBy(-vec * (m_dDis / 2));

			CString disText;
			int iDistance = ceil(m_dDis);
			disText.Format(_T("%d"), iDistance);

			centerpoint.transformBy(movevec * 300);
			AcDbObjectId dimId;
			dimId = createDimAligned(movePt1, movePt2, centerpoint, disText);
			CEquipmentroomTool::setEntToLayer(dimId, sLanesDimLayer);
			pLine->close();
			pEnt->close();
		}
		pEnt->close();
	}
}

AcDbObjectId CDlgWaiting::createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText)
{
	CString str = _T("车道轴网尺寸标注样式");
	AcDbObjectId id;
	////获得当前图形的标注样式表  
	AcDbDimStyleTable* pDimStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForRead);
	if (pDimStyleTbl->has(str))
		pDimStyleTbl->getAt(str, id);
	else
	{
		pDimStyleTbl->close();
		return id;
	}

	pDimStyleTbl->close();

	AcDbAlignedDimension *pnewdim = new AcDbAlignedDimension(pt1, pt2, ptLine, NULL, id);//创建标注实体
	AcDbObjectId dimid;
	AcDbObjectId dimensionId;
	DBHelper::AppendToDatabase(dimensionId, pnewdim);

	if (pnewdim)
		pnewdim->close();

	return dimensionId;
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
	int code = HTTP_CLIENT::Ins().get(sendUrl, true);

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

