#pragma once
#include <vector>

#define TolerantValue 0.5 //duoble�Ƚ����
class CEquipmentroomTool
{
public:
	CEquipmentroomTool();
	~CEquipmentroomTool();
	static inline bool Equal(double i1, double i2)//�Ƚ�double������
	{
		double i = i1 - i2;
		return (i >= -TolerantValue) && (i <= TolerantValue);
	}

	/*�������������������*/
	static AcDbObjectIdArray createArea(double areaSize, CString areaName, double& sideLength);
	/*����λ�ƣ�ʹ��������豸��*/
	static void textMove(AcGePoint3d ptInsert, AcDbObjectId textId);

	static ads_real getTotalArea(CString totalName);
	static AcDbObjectId CreateText(const AcGePoint3d& ptInsert, CString text, double height,
		AcDbObjectId style = AcDbObjectId::kNull, double rotation = 0);
	static AcDbObjectId CreateMText(const AcGePoint3d& ptInsert,
		CString text ,double height = 2.5, double width = 10, AcDbObjectId style = AcDbObjectId::kNull);
	/*jigչʾЧ��*/
	static void jigShow(AcDbObjectIdArray useJigIds, double sideLength);

	/*��������*/
	static void AdsorbentShow(AcDbObjectIdArray useJigIds, AcGePoint2d basePoint, double sideLength);
	/*������ֵ�������Ӧdouble����*/
	static void point2dToDouble(double minPt[2], double maxPt[2], AcGePoint2d& MinPoint, AcGePoint2d& MaxPoint);
	/*3d��ת2d��*/
	static AcGePoint2d point3dTo2d(AcGePoint3d targetPt);
	/*����һ���̶��߶κ�һ��ƽ���������߶Σ��������̶��߶�����ľ���*/
	static double getMinDistance(AcGePoint2d squarePoint, std::vector<AcGePoint2dArray>& targetLines);
	/*�Ƚ�������λ�����Ƿ����*/
	static bool compareVec(AcGeVector2d vec1, AcGeVector2d vec2);
	/*�����ֲ��ƶ�Ч������*/
	static void moveTest(AcDbObjectIdArray useJigIds, AcGeVector2d moveVec, double moveDistance);
	/*ͨ���˵�Ƚ��ж���ȡ��ֱ���Ƿ�Ϊ���ɵĶ���εı�*/
	static bool isSquareSide(AcGePoint2dArray targetLinePoints, AcGePoint2dArray squarePoints);
	/*�Ƚ�����2d���Ƿ����*/
	static bool comparePoint(AcGePoint2d a, AcGePoint2d b);

	static bool layerSet();
	static bool layerSet(const CString& layerName, const int& layerColor);
	static void setEntToLayer(AcDbObjectIdArray objectIds);
	static bool isLayerClose(AcDbEntity *pEnt);
	//add by yangbing on 2019/11/29
	/*�������*/
	static AcDbObjectId CreateHatch( const CString& patName, const AcGePoint2dArray& allPlinePts, const AcGeDoubleArray& bulges);
};

