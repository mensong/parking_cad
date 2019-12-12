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

#ifndef PI
#define PI 3.1415926535898
#endif

std::string CDlgWaiting::ms_uuid;
bool CDlgWaiting::ms_bUseV1;
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

void CDlgWaiting::setUuid(const std::string& uuid, const bool& useV1)
{
	ms_uuid = uuid;
	ms_bUseV1 = useV1;
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

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_GIF, m_ctrlGif);
	DDX_Control(pDX, IDC_STA_STATUS, m_staStatusText);
}


BOOL CDlgWaiting::OnInitDialog()
{
	BOOL bRet = CAcUiDialog::OnInitDialog();

	AcString strFile = DBHelper::GetArxDir() + _T("\\loading.gif");
	if (m_ctrlGif.Load(strFile.constPtr()))
	{
		m_ctrlGif.Draw();
	}

	m_bIsReady = true;

	SetTimer(1, 1000, NULL);

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
	//FILE* f = fopen("d:\\result.json", "r");
	//fseek(f, 0, SEEK_END);
	//size_t fileLen = ftell(f);
	//char* fileText = new char[fileLen + 1];
	//fread(fileText, 1, fileLen, f);
	//fileText[fileLen] = '\0';
	//json = fileText;
	//delete[] fileText;
	if (nIDEvent == 1)
	{
		//调用检查是否计算完成函数（）；
		std::string sMsg;
		CString sIndex;
		int status = getStatus(json, sMsg, sIndex);
		//如果完成后
		if (status == 2)
		{
			KillTimer(nIDEvent);
			//CDlgWaiting::Show(false);
			this->OnOK();
			CString sMsg;
			if (!getDataforJson(json, sMsg))
			{
				acedAlert(sMsg);
			}
			acutPrintf(_T("the uuid is :%s"), GL::Ansi2WideByte(ms_uuid.c_str()).c_str());
			DBHelper::CallCADCommandEx(_T("Redraw"));
		}
		else if (status == 0)
		{
			m_sStatus = _T("任务正在排队中，当前排在第") + sIndex + _T("位。");
			m_staStatusText.SetWindowText(m_sStatus);
		}
		else if (status == 1)
		{
			m_sStatus = _T("任务正在进行中……");
			m_staStatusText.SetWindowText(m_sStatus);
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

	typedef void(*FN_setTimeout)(int timeout);
	FN_setTimeout fn_setTimeout = ModulesManager::Instance().func<FN_setTimeout>(getHttpModule(), "setTimeout");
	if (fn_setTimeout)
	{
		fn_setTimeout(600);
	}

	typedef int(*FN_get)(const char* url, bool dealRedirect);
	FN_get fn_get = ModulesManager::Instance().func<FN_get>(getHttpModule(), "get");
	if (!fn_get)
	{
		sMsg = "get Http模块加载失败！";
		return 5;
	}
	int code = fn_get(sendUrl, true);

	if (code != 200)
	{
		char szCode[10];
		sprintf(szCode, "%d", code);
		sMsg = tempUrl + ":网络或服务器错误。(" + szCode + ")";
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	typedef const char* (*FN_getBody)(int&);
	FN_getBody fn_getBody = ModulesManager::Instance().func<FN_getBody>(getHttpModule(), "getBody");
	if (!fn_getBody)
	{
		sMsg = "getBody Http模块加载失败！";
		return 5;
	}
	int len = 0;
	json = fn_getBody(len);

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
		sIndex.Format(_T("%d"), index);
		sMsg = GL::Utf82Ansi(root["message"].asString().c_str()).c_str();
		return status;
	}

	sMsg = "结果格式有误。";
	return 3;
}

void CDlgWaiting::parkingShow(const AcGePoint2d& parkingShowPt, const double& parkingShowRotation, const CString& blockName)
{
	layerSet(_T("parkings"), 254);
	AcDbObjectId ttId;
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);
	AcGeMatrix3d mat;
	AcGeVector3d vec(0, 0, 1);
	mat.setToRotation(parkingShowRotation, vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(ttId, blockName, mat);
	setEntToLayer(ttId, _T("parkings"));
}

void CDlgWaiting::axisShow(const AcGePoint2dArray& axisPts)
{
	layerSet(_T("axis"), 1);
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId axisId;
	DBHelper::AppendToDatabase(axisId,pLine);
	pLine->close();
	setEntToLayer(axisId, _T("axis"));
}

void CDlgWaiting::laneShow(const AcGePoint2dArray& lanePts)
{
	layerSet(_T("lane"), 30);
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	AcDbObjectId laneId;
	DBHelper::AppendToDatabase(laneId,pLine);
	pLine->close();
	setEntToLayer(laneId, _T("lane"));
}

void CDlgWaiting::scopeShow(const AcGePoint2dArray& park_columnPts)
{
	layerSet(_T("scope"), 6);
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
	setEntToLayer(scopeId, _T("scope"));
}

void CDlgWaiting::pillarShow(const AcGePoint2dArray& onePillarPts)
{
	layerSet(_T("pillar"), 2);
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
	setEntToLayer(pillarId, _T("pillar"));
}

void CDlgWaiting::arrowShow(const AcGePoint2dArray& oneArrowPts)
{
	layerSet(_T("arrow"), 7);
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
	setEntToLayer(arrowId, _T("arrow"));
}

bool CDlgWaiting::layerSet(const CString& layerName, const int& layerColor)
{
	//判断有就退出无就生成图层生，并设置颜色
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return false;
	}
	if (pLayerTbl->has(layerName))//判断已经有了该图层，应置为当前图层
	{
		AcDbObjectId layerId;
		if (pLayerTbl->getAt(layerName, layerId) != Acad::eOk)
		{
			pLayerTbl->close();
			return false;
		}
		es = acdbCurDwg()->setClayer(layerId);//设为当前图层
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(layerName, pLayerTblRcd, AcDb::kForWrite);
		AcCmColor color;//设置图层颜色
		color.setColorIndex(layerColor);
		pLayerTblRcd->setColor(color);
		pLayerTblRcd->close();
		pLayerTbl->close();
		if (es != eOk)
		{
			return false;
		}
		return true;
	}

	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTblRcd->setName(layerName);
	AcDbObjectId layerTblRcdId;
	pLayerTbl->add(layerTblRcdId, pLayerTblRcd);

	AcCmColor color;//设置图层颜色
	color.setColorIndex(layerColor);
	pLayerTblRcd->setColor(color);
	// 将新建的层表记录添加到层表中
	pLayerTblRcd->close();
	pLayerTbl->close();
	es = acdbCurDwg()->setClayer(layerTblRcdId);//设为当前图层
	if (es != eOk)
	{
		return false;
	}
	return true;
}

void CDlgWaiting::setEntToLayer(const AcDbObjectId& entId,const CString& strLayerName)
{
	AcDbEntity *pEnty = NULL;
	AcDbObjectId idEnty = entId; //传如打开的实体ID，事先必须先获取到。否则会打开失败
	Acad::ErrorStatus es;
	es = acdbOpenObject(pEnty, idEnty, AcDb::kForWrite);
	if (es == Acad::eOk)
	{
		pEnty->setLayer(strLayerName); //设置实体所在的图层
		pEnty->close();
	}
	//打开失败不需要关闭实体
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

bool CDlgWaiting::getDataforJson(const std::string& json, CString& sMsg)
{
	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	AcGePoint2dArray parkingPts;
	std::vector<double> parkingDirections;
	std::vector<AcGePoint2dArray> axisesPoints;
	std::vector<AcGePoint2dArray> lanesPoints;
	AcGePoint2dArray scopePts;
	std::vector<AcGePoint2dArray> pillarPoints;
	std::vector<AcGePoint2dArray> arrowPoints;
	double dParkingLength;
	double dParkingWidth;
	double dLaneWidth;
	if (reader.parse(json, root))
	{
		Json::Value& parkings = root["result"]["parkings"];
		if (parkings.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"parkings\"]字段！");
			return false;
		}
		else
		{
			if (parkings.isArray())
			{
				int nArraySize = parkings.size();
				for (int i = 0; i < nArraySize; i++)
				{
					double ptX = parkings[i]["position"][0].asDouble();
					double ptY = parkings[i]["position"][1].asDouble();
					AcGePoint2d pt(ptX, ptY);
					parkingPts.append(pt);
					double direction = parkings[i]["direction"].asDouble();
					parkingDirections.push_back(direction);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"parkings\"]字段格式不匹配！");
				return false;
			}
		}
		Json::Value& data = root["result"]["data"];
		if (data.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"data\"]字段！");
			return false;
		}
		else
		{
			if (data["cell_length"].isDouble() && data["cell_num"].isInt())
			{
				dParkingLength = data["cell_length"].asDouble();
				dParkingWidth = data["cell_width"].asDouble();
				dLaneWidth = data["lane_width"].asDouble();
				std::vector<double> dataVector;
				std::vector<std::string> strDataNameVector;
				std::map < std::string , double > tableData;

				double dSp = data["Sp"].asDouble();
				strDataNameVector.push_back("SP");
				dataVector.push_back(dSp);

				double dSpt = data["Spt"].asDouble();
				strDataNameVector.push_back("SPT");
				dataVector.push_back(dSpt);

				double dSpf = data["Spf"].asDouble();
				strDataNameVector.push_back("SPF");
				dataVector.push_back(dSpf);

				double dSpf1 = data["Spf1"].asDouble();
				strDataNameVector.push_back("SPF1");
				dataVector.push_back(dSpf1);

				double dSpf2 = data["Spf2"].asDouble();
				strDataNameVector.push_back("SPF2");
				dataVector.push_back(dSpf2);

				double dSpf3 = data["Spf3"].asDouble();
				strDataNameVector.push_back("SPF3");
				dataVector.push_back(dSpf3);

				double dCp = data["Cp"].asDouble();
				strDataNameVector.push_back("CP");
				dataVector.push_back(dCp);

				double dJSpc = data["JSpc"].asDouble();
				strDataNameVector.push_back("JSPC");
				dataVector.push_back(dJSpc);

				double dSpc = data["Spc"].asDouble();
				strDataNameVector.push_back("SPC");
				dataVector.push_back(dSpc);

				double dH = data["H"].asDouble();
				strDataNameVector.push_back("H");
				dataVector.push_back(dH);

				double Ht = data["Ht"].asDouble();
				strDataNameVector.push_back("HT");
				dataVector.push_back(Ht);
				CString resultText;
				for (int i = 0; i < dataVector.size(); i++)
				{
					CString strTempData;
					strTempData.Format(_T("%.2f"), dataVector[i]);
					std::pair<std::string, double> value(strDataNameVector[i], dataVector[i]);
					tableData.insert(value);//插入新元素
					//resultText += (strDataNameVector[i] + _T("=") + strTempData + _T("|"));
				}
				int gg = tableData.size();
				std::string strText = CT2A((LPCTSTR)resultText);//CString->std::string
				strText.erase(strText.end() - 1);
				COperaAddFrame::setTextStr(strText);
				std::string Textstr = "SP=4865|SPT=3740|SPF=1125|SPF1=210|SPF2=450|SPF3=463|CP=132|JSPC=25|SPC=33|H=3.55|HT=1";
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"parkings\"]字段格式不匹配！");
				return false;
			}
		}

		Json::Value& axis = root["result"]["axis"];
		if (axis.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"axis\"]字段！");
			return false;
		}
		else
		{
			if (axis.isArray())
			{
				int nAxisSize = axis.size();
				for (int j = 0; j < nAxisSize; j++)
				{
					double ptX1 = axis[j][0][0].asDouble();
					double ptY1 = axis[j][0][1].asDouble();
					double ptX2 = axis[j][1][0].asDouble();
					double ptY2 = axis[j][1][1].asDouble();
					AcGePoint2d startPt(ptX1, ptY1);
					AcGePoint2d endPt(ptX2, ptY2);
					AcGePoint2dArray axisPts;
					axisPts.append(startPt);
					axisPts.append(endPt);
					axisesPoints.push_back(axisPts);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"axis\"]字段格式不匹配！");
				return false;
			}
		}

		Json::Value& lane = root["result"]["lane"];
		if (lane.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"lane\"]字段！");
			return false;
		}
		else
		{
			if (lane.isArray())
			{
				int nLaneSize = lane.size();
				for (int m = 0; m < nLaneSize; m++)
				{
					double ptX1 = lane[m][0][0].asDouble();
					double ptY1 = lane[m][0][1].asDouble();
					double ptX2 = lane[m][1][0].asDouble();
					double ptY2 = lane[m][1][1].asDouble();
					AcGePoint2d startPt(ptX1, ptY1);
					AcGePoint2d endPt(ptX2, ptY2);
					AcGePoint2dArray lanePts;
					lanePts.append(startPt);
					lanePts.append(endPt);
					lanesPoints.push_back(lanePts);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"lane\"]字段格式不匹配！");
				return false;
			}
		}

		Json::Value& scope = root["result"]["scope"];
		if (scope.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"scope\"]字段！");
			return false;
		}
		else
		{
			if (scope.isArray())
			{
				int nPark_columSize = scope.size();
				for (int n = 0; n < nPark_columSize; n++)
				{
					double ptX = scope[n][0].asDouble();
					double ptY = scope[n][1].asDouble();
					AcGePoint2d plinePt(ptX, ptY);
					scopePts.append(plinePt);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"scope\"]字段格式不匹配！");
				return false;
			}
		}

		Json::Value& pillar = root["result"]["pillar"];
		if (pillar.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"pillar\"]字段！");
			return false;
		}
		else
		{
			if (pillar.isArray())
			{
				int npillarSize = pillar.size();
				for (int k = 0; k < npillarSize; k++)
				{
					AcGePoint2dArray onePillarPts;
					if (pillar[k].isArray())
					{
						int onepillarSize = pillar[k].size();
						for (int g = 0; g < onepillarSize; g++)
						{
							double ptX = pillar[k][g][0].asDouble();
							double ptY = pillar[k][g][1].asDouble();
							AcGePoint2d tempPt(ptX, ptY);
							onePillarPts.append(tempPt);
						}
					}
					pillarPoints.push_back(onePillarPts);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"pillar\"]字段格式不匹配！");
				return false;
			}
		}

		Json::Value& arrow = root["result"]["arrow"];
		if (arrow.isNull())
		{
			sMsg = _T("回传json不存在[\"result\"][\"arrow\"]字段！");
			return false;
		}
		else
		{
			if (arrow.isArray())
			{
				int narrowSize = arrow.size();
				for (int k = 0; k < narrowSize; k++)
				{
					AcGePoint2dArray oneArrowPts;
					if (arrow[k].isArray())
					{
						int nonearrowSize = arrow[k].size();
						for (int g = 0; g < nonearrowSize; g++)
						{
							double ptX = arrow[k][g][0].asDouble();
							double ptY = arrow[k][g][1].asDouble();
							AcGePoint2d tempPt(ptX, ptY);
							oneArrowPts.append(tempPt);
						}
					}
					arrowPoints.push_back(oneArrowPts);
				}
			}
			else
			{
				sMsg = _T("回传json中[\"result\"][\"arrow\"]字段格式不匹配！");
				return false;
			}
		}
	}
	else
	{
		sMsg = _T("解析回传json文件出错！");
		return false;
	}
	Doc_Locker _locker;
	layerSet(_T("0"), 7);
	CString blockName;
	creatNewParking(dParkingLength, dParkingWidth, blockName);
	for (int a = 0; a < parkingPts.length(); a++)
	{
		//double = (rotation/180)*Π(顺时针和逆时针)
		double rotation = ((360 - parkingDirections[a]) / 180)*ARX_PI;
		parkingShow(parkingPts[a], rotation, blockName);
	}

	for (int b = 0; b < axisesPoints.size(); b++)
	{
		axisShow(axisesPoints[b]);
	}

	for (int c = 0; c < lanesPoints.size(); c++)
	{
		laneShow(lanesPoints[c]);
	}

	scopeShow(scopePts);

	for (int d = 0; d < pillarPoints.size(); d++)
	{
		pillarShow(pillarPoints[d]);
	}

	for (int e = 0; e < arrowPoints.size(); e++)
	{
		arrowShow(arrowPoints[e]);
	}
	//生成车道标注
	double dTransLaneWidth = dLaneWidth * 1000;
	if (dTransLaneWidth == 0)
	{
		acutPrintf(_T("车道宽度数据有误！"));
	}
	else
	{
		setLandDismensions(dTransLaneWidth, _T("lane"));
	}
	DBHelper::CallCADCommand(_T("ANM "));
	//setLayerClose(_T("axis"));
	//setLayerClose(_T("轴网标注"));
	layerSet(_T("0"), 7);
	return true;
}

void CDlgWaiting::setLandDismensions(double m_dDis, const AcString& CarLaneLayerName)
{
	layerSet(_T("lanesDim"), 7);
	AcDbObjectIdArray RoadLineIds = DBHelper::GetEntitiesByLayerName(CarLaneLayerName);
	if (RoadLineIds.length() == 0)
	{
		acutPrintf(_T("没有车道信息，生成车道标注失败！"));
		return;
	}
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es;
	for (int i = 0; i < RoadLineIds.length(); ++i)
	{
		es = acdbOpenObject(pEnt, RoadLineIds[i], AcDb::kForRead);
		if (es != eOk)
			continue;

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
		AcGeVector3d vec = tempVec.rotateBy(PI / 2, AcGeVector3d(0, 0, 1));
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
		setEntToLayer(dimId,_T("lanesDim"));
		if (pEnt)
			pEnt->close();
	}
}

AcDbObjectId CDlgWaiting::createDimAligned(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine, const ACHAR* dimText)
{
	/* 对齐标注：AcDbAlignedDimension类:
	第一个参数：xLine1Point：第一条尺寸边界线的起点；第二个参数：xLine2Point：第二条尺寸边界线的起点：
	第三个参数：dimLinePoint：通过尺寸线的一点；第四个参数：dimText ：标注文字；第五个参数：dimStyle ：样式。*/
	AcDbAlignedDimension *pDim = new AcDbAlignedDimension(pt1, pt2, ptLine, dimText, AcDbObjectId::kNull);
	AcDbObjectId dimensionId;
	DBHelper::AppendToDatabase(dimensionId,pDim);
	pDim->close();
	// 打开已经创建的标注，对文字的位置进行修改
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, dimensionId, AcDb::kForWrite);

	AcDbAlignedDimension *pDimension = AcDbAlignedDimension::cast(pEnt);
	if (pDimension != NULL)
	{
		// 移动文字位置前，设置文字和尺寸线移动时的关系（这里指定为：尺寸线不动，在文字和尺寸线之间加箭头）
		pDimension->setDimtmove(1);

		pDimension->setDimblk(_T("_OPEN"));//设置箭头的形状为建筑标记
		pDimension->setDimexe(5);//设置尺寸界线超出尺寸线距离为400
		pDimension->setDimexo(0);//设置尺寸界线的起点偏移量为300
								 //pDimension->setDimtad(50);//文字位于标注线的上方
		pDimension->setDimtix(0);//设置标注文字始终绘制在尺寸界线之间
		pDimension->setDimtxt(5);//标注文字的高度
		pDimension->setDimdec(2);
		pDimension->setDimasz(1);//箭头长度
		pDimension->setDimlfac(1);//比例因子
		AcCmColor suiceng;
		suiceng.setColorIndex(3);
		pDimension->setDimclrd(suiceng);//为尺寸线、箭头和标注引线指定颜色，0为随图层
		pDimension->setDimclre(suiceng);//为尺寸界线指定颜色。此颜色可以是任意有效的颜色编号
	}
	pEnt->close();
	return dimensionId;
}


