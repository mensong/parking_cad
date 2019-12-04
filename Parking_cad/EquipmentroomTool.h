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
	static AcDbObjectIdArray createArea(double areaSize, CString areaName, double& sideLength);
	/*文字位移，使其居中于设备房*/
	static void textMove(AcGePoint3d ptInsert, AcDbObjectId textId);

	static ads_real getTotalArea(CString totalName);
	static AcDbObjectId CreateText(const AcGePoint3d& ptInsert, CString text, double height,
		AcDbObjectId style = AcDbObjectId::kNull, double rotation = 0);
	static AcDbObjectId CreateMText(const AcGePoint3d& ptInsert,
		CString text ,double height = 2.5, double width = 10, AcDbObjectId style = AcDbObjectId::kNull);
	/*jig展示效果*/
	static void jigShow(AcDbObjectIdArray useJigIds, double sideLength);

	/*吸附功能*/
	static void AdsorbentShow(AcDbObjectIdArray useJigIds, AcGePoint2d basePoint, double sideLength);
	/*输入最值点输出对应double数组*/
	static void point2dToDouble(double minPt[2], double maxPt[2], AcGePoint2d& MinPoint, AcGePoint2d& MaxPoint);
	/*3d点转2d点*/
	static AcGePoint2d point3dTo2d(AcGePoint3d targetPt);
	/*输入一个固定线段和一组平行与它的线段，输出距离固定线段最近的距离*/
	static double getMinDistance(AcGePoint2d squarePoint, std::vector<AcGePoint2dArray>& targetLines);
	/*比较两个单位向量是否相等*/
	static bool compareVec(AcGeVector2d vec1, AcGeVector2d vec2);
	/*吸附分步移动效果测试*/
	static void moveTest(AcDbObjectIdArray useJigIds, AcGeVector2d moveVec, double moveDistance);
	/*通过端点比较判断所取得直线是否为构成的多边形的边*/
	static bool isSquareSide(AcGePoint2dArray targetLinePoints, AcGePoint2dArray squarePoints);
	/*比较两个2d点是否相等*/
	static bool comparePoint(AcGePoint2d a, AcGePoint2d b);

	static bool layerSet();
	static bool layerSet(const CString& layerName, const int& layerColor);
	static void setEntToLayer(AcDbObjectIdArray objectIds);
	static bool isLayerClose(AcDbEntity *pEnt);
	//add by yangbing on 2019/11/29
	/*面域填充*/
	static AcDbObjectId CreateHatch( const CString& patName, const AcGePoint2dArray& allPlinePts, const AcGeDoubleArray& bulges);
};

