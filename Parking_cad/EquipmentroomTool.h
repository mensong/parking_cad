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
	static AcDbObjectIdArray createArea(double areaSize, CString areaName, double& sideLength, double& limitLength, bool limitType=true);
	/*����λ�ƣ�ʹ��������豸��*/
	static void textMove(AcGePoint3d ptInsert, AcDbObjectId textId);

	static bool getTotalArea(CString totalName, ads_real& totalArea);
	static AcDbObjectId CreateText(const AcGePoint3d& ptInsert, CString text, double height,
		AcDbObjectId style = AcDbObjectId::kNull, double rotation = 0);
	static AcDbObjectId CreateMText(const AcGePoint3d& ptInsert,
		CString text ,double height = 2.5, double width = 10, AcDbObjectId style = AcDbObjectId::kNull);
	/*jigչʾЧ��*/
	static bool jigShow(AcDbObjectIdArray useJigIds, double sideLength);

	/*��������*/
	static void AdsorbentShow(AcDbObjectIdArray useJigIds, AcGePoint2d basePoint, double sideLength);
	/*������ֵ�������Ӧdouble����*/
	static void point2dToDouble(double minPt[2], double maxPt[2], AcGePoint2d& MinPoint, AcGePoint2d& MaxPoint);
	/*3d��ת2d��*/
	static AcGePoint2d point3dTo2d(AcGePoint3d targetPt);
	/*����һ���̶��߶κ�һ��ƽ���������߶Σ��������̶��߶�����ľ���*/
	static double getMinDistance(AcGePoint2d squarePoint, std::vector<AcGePoint2dArray>& targetLines);
	/*�����ֲ��ƶ�Ч������*/
	static void moveTest(AcDbObjectIdArray useJigIds, AcGeVector2d moveVec, double moveDistance);
	/*ͨ���˵�Ƚ��ж���ȡ��ֱ���Ƿ�Ϊ���ɵĶ���εı�*/
	static bool isSquareSide(AcGePoint2dArray targetLinePoints, AcGePoint2dArray squarePoints);
	/*�Ƚ�����2d���Ƿ����*/
	static bool comparePoint(AcGePoint2d a, AcGePoint2d b);

	static bool layerSet();
	static bool layerSet(const CString& layerName, const int& layerColor, AcDbDatabase *pDb = acdbCurDwg());
	static void setEntToLayer(AcDbObjectIdArray objectIds);
	static void setEntToLayer(const AcDbObjectId& entId, const CString& strLayerName);
	static bool isLayerClose(AcDbEntity *pEnt);
	//add by yangbing on 2019/11/29
	/*�������*/
	static AcDbObjectId CreateHatch( const CString& patName, const AcGePoint2dArray& allPlinePts, const AcGeDoubleArray& bulges);
	/*����������ʽ*/
	static void creatTextStyle(CString& textStyleName);
	/*�������ļ���ȡͼ����*/
	static std::string getLayerName(const std::string& strLayer);
	static std::string getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key);
	static void getJsonInformation(const std::string& inputroot, const std::string& object, const std::string& key, std::vector<std::string>& arrayvector);
	/*ɾ��ָ��ͼ��*/
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

	//�豸�������������
	static double areaScale(double oldArea);
	//ȡ�ó�λ��Χ���
	static void getParkingExtentPts(std::vector<AcGePoint2dArray>& parkingExtentPts, const std::vector<AcDbObjectId>& allChooseIds, const CString& parkingLayerName, std::map<AcDbObjectId,AcGePoint2dArray>& parkIdAndPts);
	static CString getOpenDwgFilePath();
	//�෽���Ա�
	static AcGePoint2dArray getAllEntCreatExten();
	//��ӡ��ǰʱ��
	static void pritfCurTime();
	static bool hasNameOfBlock(CString sBlockName);
};

