// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include <atomic>
#include "DBHelper.h"
#include "ArxDialog.h"
#include "..\HttpHelper\HttpHelper.h"
#include "Convertor.h"
#include <json/json.h>

std::string CDlgWaiting::ms_uuid;

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

	if (g_dlg == NULL)
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
}


BOOL CDlgWaiting::OnInitDialog()
{
	BOOL bRet = CAcUiDialog::OnInitDialog();

	CString strFile = DBHelper::GetArxDir() + _T("\\loading.gif");
	if (m_ctrlGif.Load(strFile))
	{
		m_ctrlGif.Draw();
	}

	m_bIsReady = true;

	SetTimer(1, 2000, NULL);
	
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
		CString sMsg;
		int status = getStatus(json, sMsg);
		//如果完成后
		if (status==2)
		{
			KillTimer(nIDEvent);
			CDlgWaiting::Show(false);

			Json::Reader reader;
			Json::Value root;
			//从字符串中读取数据
			AcGePoint2dArray parkingPts;
			std::vector<double> parkingDirections;
			std::vector<AcGePoint2dArray> axisesPoints;
			std::vector<AcGePoint2dArray> lanesPoints;
			AcGePoint2dArray scopePts;
			std::vector<AcGePoint2dArray> pillarPoints;
			if (reader.parse(json, root))
			{

				if (root["result"]["parkings"].isArray())
				{
					int nArraySize = root["result"]["parkings"].size();
					for (int i = 0; i < nArraySize; i++)
					{
						double ptX = root["result"]["parkings"][i]["position"][0].asDouble();
						double ptY = root["result"]["parkings"][i]["position"][1].asDouble();
						AcGePoint2d pt(ptX,ptY);
						parkingPts.append(pt);
						double direction = root["result"]["parkings"][i]["direction"].asDouble();
						parkingDirections.push_back(direction);
					}
				}

				if (root["result"]["axis"].isArray())
				{
					int nAxisSize = root["result"]["axis"].size();
					for (int j = 0;j < nAxisSize;j++)
					{
						double ptX1 = root["result"]["axis"][j][0][0].asDouble();
						double ptY1 = root["result"]["axis"][j][0][1].asDouble();
						double ptX2 = root["result"]["axis"][j][1][0].asDouble();
						double ptY2 = root["result"]["axis"][j][1][1].asDouble();
						AcGePoint2d startPt(ptX1,ptY1);
						AcGePoint2d endPt(ptX2,ptY2);
						AcGePoint2dArray axisPts;
						axisPts.append(startPt);
						axisPts.append(endPt);
						axisesPoints.push_back(axisPts);
					}
				}

				if (root["result"]["lane"].isArray())
				{
					int nLaneSize = root["result"]["lane"].size();
					for (int m = 0; m < nLaneSize; m++)
					{
						double ptX1 = root["result"]["lane"][m][0][0].asDouble();
						double ptY1 = root["result"]["lane"][m][0][1].asDouble();
						double ptX2 = root["result"]["lane"][m][1][0].asDouble();
						double ptY2 = root["result"]["lane"][m][1][1].asDouble();
						AcGePoint2d startPt(ptX1, ptY1);
						AcGePoint2d endPt(ptX2, ptY2);
						AcGePoint2dArray lanePts;
						lanePts.append(startPt);
						lanePts.append(endPt);
						lanesPoints.push_back(lanePts);
					}
				}

				if (root["result"]["scope"].isArray())
				{
					int nPark_columSize = root["result"]["scope"].size();
					for (int n = 0;n < nPark_columSize;n++)
					{
						double ptX = root["result"]["scope"][n][0].asDouble();
						double ptY = root["result"]["scope"][n][1].asDouble();
						AcGePoint2d plinePt(ptX,ptY);
						scopePts.append(plinePt);
					}
				}
				if (root["result"]["pillar"].isArray())
				{
					int npillarSize = root["result"]["pillar"].size();
					for (int k = 0;k < npillarSize;k++)
					{
						AcGePoint2dArray onePillarPts;
						if (root["result"]["pillar"][k].isArray())
						{
							int onepillarSize = root["result"]["pillar"][k].size();
							for (int g = 0;g < onepillarSize;g++)
							{
								double ptX = root["result"]["pillar"][k][g][0].asDouble();
								double ptY = root["result"]["pillar"][k][g][1].asDouble();
								AcGePoint2d tempPt(ptX, ptY);
								onePillarPts.append(tempPt);
							}				
						}
						pillarPoints.push_back(onePillarPts);
					}
				}

			}

			Doc_Locker _locker;

			for (int a = 0;a < parkingPts.length();a++)
			{
				//double = (rotation/180)*Π(顺时针和逆时针)
				double rotation = ((360-parkingDirections[a]) / 180)*3.1415926;
				parkingShow(parkingPts[a], rotation);
			}
			for (int b = 0;b < axisesPoints.size();b++)
			{
				axisShow(axisesPoints[b]);
			}
			for (int c = 0;c < lanesPoints.size();c++)
			{
				laneShow(lanesPoints[c]);
			}
			scopeShow(scopePts);
			for (int d = 0;d < pillarPoints.size();d++)
			{
				pillarShow(pillarPoints[d]);
			}
			setAxisLayerClose();
		}
		else if(status==3)
		{
			KillTimer(nIDEvent);
			acedAlert(sMsg);
			CDlgWaiting::Show(false);
		}
		return;
	}

	CAcUiDialog::OnTimer(nIDEvent);
}

int CDlgWaiting::getStatus(std::string& json, CString& sMsg)
{
	if (ms_uuid == "")
	{
		sMsg = _T("uuid不能为空。");
		return 3;
	}

	//std::map<std::string, std::string> fields;
	//fields.insert(std::pair<std::string, std::string>("uuid",uuid));

	HttpHelper http;
	//int code = http.post("http://10.8.212.187/park", sData.c_str(), sData.size(), true, "application/json");
	
	//int code = http.get("http://10.8.212.187/park",fields,true);

	std::string httpUrl = "http://10.8.212.187/query/";
	std::string tempUrl = httpUrl + ms_uuid;
	const char * sendUrl = tempUrl.c_str();
	int code = http.get(sendUrl, true);

	if (code != 200)
	{
		sMsg = _T("网络或服务器错误。");
		return 3;
	}
	//std::string sRes = GL::Utf82Ansi(http.response.body.c_str());

	json = http.response.body;

	Json::Reader reader;
	Json::Value root;
	//从字符串中读取数据
	if (reader.parse(json, root))
	{
#ifdef _DEBUG
		WriteFile("result.json", json.c_str(), json.size(), NULL, 0, false);
#endif

		int status = root["status"].asInt();
		sMsg = GL::Utf82WideByte(root["message"].asString().c_str()).c_str();
		return status;
	}
	else
	{
		sMsg = _T("结果格式有误。");
		return 3;
	}
}

void CDlgWaiting::parkingShow(AcGePoint2d& parkingShowPt,double& parkingShowRotation)
{
	AcDbObjectId ttId;
	AcGeVector3d pt(parkingShowPt.x, parkingShowPt.y, 0);

	layerSet(_T("parkings"), 254);

	AcGeMatrix3d mat;
	AcGeVector3d vec(0,0,1);
	mat.setToRotation(parkingShowRotation,vec);//double = (rotation/180)*Π
	mat.setTranslation(pt);
	DBHelper::InsertBlkRef(ttId,_T("parking_1"), mat);
} 

void CDlgWaiting::axisShow(AcGePoint2dArray& axisPts)
{
	layerSet(_T("axis"), 1);
	AcGePoint3d ptStart(axisPts[0].x, axisPts[0].y, 0);
	AcGePoint3d ptEnd(axisPts[1].x, axisPts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	DBHelper::AppendToDatabase(pLine);
	pLine->close();
}

void CDlgWaiting::laneShow(AcGePoint2dArray& lanePts)
{
	layerSet(_T("lane"), 30);
	AcGePoint3d ptStart(lanePts[0].x, lanePts[0].y, 0);
	AcGePoint3d ptEnd(lanePts[1].x, lanePts[1].y, 0);
	AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);
	DBHelper::AppendToDatabase(pLine);
	pLine->close();
}

void CDlgWaiting::scopeShow(AcGePoint2dArray& park_columnPts)
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

void CDlgWaiting::pillarShow(AcGePoint2dArray& onePillarPts)
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

bool CDlgWaiting::layerSet(CString layerName,int layerColor)
{
	// 判断是否存在名称为“设备房”的图层
	AcDbLayerTable *pLayerTbl;
	//获取当前图形层表
	Acad::ErrorStatus es;
	es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
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
		es = acdbHostApplicationServices()->workingDatabase()->setClayer(layerId);//设为当前图层
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
	es = acdbHostApplicationServices()->workingDatabase()->setClayer(layerTblRcdId);//设为当前图层
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
	es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
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
