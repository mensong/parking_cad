#pragma once
#include <vector>

#define TolerantValue 0.5 //duoble比较误差

class CEquipmentroomTool
{
public:
	CEquipmentroomTool();
	~CEquipmentroomTool();
	static inline bool Equal(double i1, double i2)//比较double型数据
	{
		double i = i1 - i2;
		return (i >= -TolerantValue) && (i <= TolerantValue);
	}

	/*根据输入面积创建面域*/
	static AcDbObjectIdArray createArea(double areaSize, CString areaName, double& sideLength, double& limitLength, bool limitType=true);
	/*文字位移，使其居中于设备房*/
	static void textMove(AcGePoint3d ptInsert, AcDbObjectId textId);

	static bool getTotalArea(CString totalName, ads_real& totalArea);
	static AcDbObjectId CreateText(const AcGePoint3d& ptInsert, CString text, double height,
		AcDbObjectId style = AcDbObjectId::kNull, double rotation = 0);
	static AcDbObjectId CreateMText(const AcGePoint3d& ptInsert,
		CString text ,double height = 2.5, double width = 10, AcDbObjectId style = AcDbObjectId::kNull);
	/*jig展示效果*/
	static bool jigShow(AcDbObjectIdArray useJigIds, double sideLength);

	/*吸附功能*/
	static void AdsorbentShow(AcDbObjectIdArray useJigIds, AcGePoint2d basePoint, double sideLength);
	/*输入最值点输出对应double数组*/
	static void point2dToDouble(double minPt[2], double maxPt[2], AcGePoint2d& MinPoint, AcGePoint2d& MaxPoint);
	/*3d点转2d点*/
	static AcGePoint2d point3dTo2d(AcGePoint3d targetPt);
	/*输入一个固定线段和一组平行与它的线段，输出距离固定线段最近的距离*/
	static double getMinDistance(AcGePoint2d squarePoint, std::vector<AcGePoint2dArray>& targetLines);
	/*吸附分步移动效果测试*/
	static void moveTest(AcDbObjectIdArray useJigIds, AcGeVector2d moveVec, double moveDistance);
	/*通过端点比较判断所取得直线是否为构成的多边形的边*/
	static bool isSquareSide(AcGePoint2dArray targetLinePoints, AcGePoint2dArray squarePoints);
	/*比较两个2d点是否相等*/
	static bool comparePoint(AcGePoint2d a, AcGePoint2d b);

	static bool layerSet();
	static bool layerSet(const CString& layerName, const int& layerColor, AcDbDatabase *pDb = acdbCurDwg());
	static void setEntToLayer(AcDbObjectIdArray objectIds);
	static void setEntToLayer(const AcDbObjectId& entId, const CString& strLayerName);
	static bool isLayerClose(AcDbEntity *pEnt);
	//add by yangbing on 2019/11/29
	/*面域填充*/
	static AcDbObjectId CreateHatch( const CString& patName, const AcGePoint2dArray& allPlinePts, const AcGeDoubleArray& bulges);
	/*创建文字样式*/
	static void creatTextStyle(CString& textStyleName);
	/*从配置文件获取图层名*/
	static std::string getLayerName(const std::string& strLayer);
	static std::string getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key);
	static void getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key, std::vector<std::string>& arrayvector);
	/*删除指定图层*/
	static bool deletLayerByName(const CString& layerNaem); 
	static Acad::ErrorStatus deletLayer(AcDbLayerTableRecord* pLTR, AcDbLayerTable* pLT = NULL);
	static void setLayerClose(const CString& layerName,AcDbDatabase *pDb = acdbCurDwg());
	static bool isLayerClosed(const CString& strLayerName);
	static void setLayerOpen(const CString& strLayerName);
	static bool getLayerConfigForJson(const std::string& sLayerInfo, std::string& sProfessionalAttributes, std::string& sLayerName,
		std::string& sLayerColor,std::string& sLayerLinetype, std::string& sLayerWidth,std::string& sIsPrintf, std::string& sTransparency);

	static bool layerConfigSet(const CString& layerName, const CString& layerColor, const CString& lineWidth, const CString& lineType, const CString& transparency, const CString& isPrint,AcDbDatabase *pDb = acdbCurDwg());
	static std::string getLayerNameByJson(const std::string& sLayerInfo);
	static int SelColor(int& textColor);
	static void creatLayerByjson(const std::string& sLayerInfo,AcDbDatabase *pDb = acdbCurDwg());

	//设备房面积比例调整
	static double areaScale(double oldArea);
	//取得车位包围框点
	static void getParkingExtentPts(std::vector<AcGePoint2dArray>& parkingExtentPts, const std::vector<AcDbObjectId>& allChooseIds, const CString& parkingLayerName, std::map<AcDbObjectId,AcGePoint2dArray>& parkIdAndPts);
	static CString getOpenDwgFilePath();
	//多方案对比
	static AcGePoint2dArray getAllEntCreatExten();
	//打印当前时间
	static void pritfCurTime();
	static bool hasNameOfBlock(CString sBlockName);
};

