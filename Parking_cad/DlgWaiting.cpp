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

std::string CDlgWaiting::ms_uuid;
std::string CDlgWaiting::ms_geturl;

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

void CDlgWaiting::setUuid(const std::string& uuid)
{
	ms_uuid = uuid;
}

void CDlgWaiting::setGetUrl(const std::string& geturl)
{
	ms_geturl = geturl;
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
		if (status==2)
		{
			KillTimer(nIDEvent);
			//CDlgWaiting::Show(false);
			this->OnOK();
			CString sMsg;
			if (!getDataforJson(json, sMsg))
			{
				acedAlert(sMsg);
			}
			DBHelper::CallCADCommandEx(_T("Redraw"));
		}
		else if(status==0)
		{
			m_sStatus = _T("任务正在排队中，当前排在第") + sIndex + _T("位。");
			m_staStatusText.SetWindowText(m_sStatus);
		}
		else if(status==1)
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

int CDlgWaiting::getStatus(std::string& json, std::string& sMsg ,CString& sIndex)
{
	if (ms_uuid == "")
	{
		sMsg = "uuid不能为空。";
		return 4;
	}

	//std::string httpUrl = "http://10.8.212.187/query/";
	std::string tempUrl = ms_geturl + ms_uuid;
	const char * sendUrl = tempUrl.c_str();
	
	typedef void (*FN_setTimeout)(int timeout);
	FN_setTimeout fn_setTimeout = ModulesManager::Instance().func<FN_setTimeout>(getHttpModule(), "setTimeout");
	if (fn_setTimeout)
	{
		fn_setTimeout(600);
	}

	typedef int (*FN_get)(const char* url, bool dealRedirect);
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

void CDlgWaiting::parkingShow(const AcGePoint2d& parkingShowPt,const double& parkingShowRotation)
{
	AcDbObjectId ttId;
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);

	layerSet(_T("parkings"), 254);

	AcGeMatrix3d mat;
	AcGeVector3d vec(0,0,1);
	mat.setToRotation(parkingShowRotation,vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(ttId,_T("parking"), mat);
} 

void CDlgWaiting::axisShow(const AcGePoint2dArray& axisPts)
{
	layerSet(_T("axis"), 1);
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	DBHelper::AppendToDatabase(pLine);
	pLine->close();
}

void CDlgWaiting::laneShow(const AcGePoint2dArray& lanePts)
{
	layerSet(_T("lane"), 30);
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	DBHelper::AppendToDatabase(pLine);
	pLine->close();
}

void CDlgWaiting::scopeShow(const AcGePoint2dArray& park_columnPts)
{
	layerSet(_T("scope"), 6);
	AcDbPolyline *pPoly = new AcDbPolyline(park_columnPts.length());
	double width = 0;//线宽
	for (int i=0;i<park_columnPts.length();i++)
	{
		pPoly->addVertexAt(0, park_columnPts[i], 0, width, width);
	}
	pPoly->setClosed(true);
	DBHelper::AppendToDatabase(pPoly);
	pPoly->close();
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
	DBHelper::AppendToDatabase(pPoly);
	pPoly->close();
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
	DBHelper::AppendToDatabase(pPoly);
	pPoly->close();
}

bool CDlgWaiting::layerSet(const CString& layerName,const int& layerColor)
{
	// 判断是否存在名称为“设备房”的图层
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

void CDlgWaiting::setAxisLayerClose()
{
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbCurDwg()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (es != eOk)
	{
		return ;
	}
	if (pLayerTbl->has(_T("axis")))
	{
		AcDbLayerTableRecord *pLTR;
		pLayerTbl->getAt(_T("axis"), pLTR, AcDb::kForWrite);
		DBHelper::SetLayerIsOff(pLTR);
		pLTR->close();
	}
	pLayerTbl->close();
}

void CDlgWaiting::creatNewParking(const double& dParkingLength, const double& dParkingWidth)
{
	AcGePoint2d squarePt1(0, 0);
	AcGePoint2d squarePt2(0,dParkingLength);
	AcGePoint2d squarePt3(dParkingWidth, dParkingLength);
	AcGePoint2d squarePt4(dParkingWidth, 0);
	AcDbPolyline *pPoly = new AcDbPolyline(4);
	double width = 20;//矩形方形线宽
	pPoly->addVertexAt(0, squarePt1, 0, width, width);
	pPoly->addVertexAt(1, squarePt2, 0, width, width);
	pPoly->addVertexAt(2, squarePt3, 0, width, width);
	pPoly->addVertexAt(3, squarePt4, 0, width, width);
	pPoly->setClosed(true);
	AcDbObjectId squareId;
	DBHelper::AppendToDatabase(squareId, pPoly);
	AcDbObjectIdArray allIds;
	allIds.append(squareId);
	pPoly->setColorIndex(6);
	pPoly->close();
	AcGePoint3d centerPt(-(dParkingWidth /2),-(dParkingLength/2),0);
	AcDbObjectId parkingId;
	//AcGePoint3d test(-1200, -2550, 0);
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
	DBHelper::CreateBlock(_T("parking"), blockEnts, centerPt);
	for (int j=0; j<blockEnts.size(); j++)
	{
		blockEnts[j]->erase();
		blockEnts[j]->close();
	}
}

bool CDlgWaiting::getDataforJson(const std::string& json,CString& sMsg)
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
			if (data["cell_length"].isDouble()&&data["cell_num"].isInt())
			{
				dParkingLength = data["cell_length"].asDouble()*1000;
				dParkingWidth = data["cell_width"].asDouble()*1000;
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

	creatNewParking(dParkingLength, dParkingWidth);

	for (int a = 0; a < parkingPts.length(); a++)
	{
		//double = (rotation/180)*Π(顺时针和逆时针)
		double rotation = ((360 - parkingDirections[a]) / 180)*ARX_PI;
		parkingShow(parkingPts[a], rotation);
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

	setAxisLayerClose();

	return true;
}

