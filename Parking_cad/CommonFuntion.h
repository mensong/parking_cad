#pragma once

#include <vector>
#include <afxstr.h>
#include <dbmain.h>



class CCommonFuntion
{
public:
	CCommonFuntion();
	~CCommonFuntion();

public:
	static AcDbObjectIdArray Select_Ent( AcGePoint3d& centerpoint);
	static bool PromptSelectEnts(const TCHAR* prompt, AcRxClass* classDesc, AcDbObjectIdArray &entIds);
	static bool PromptSelectEnts(const TCHAR* prompt, const std::vector<AcRxClass*> &classDescs, AcDbObjectIdArray &entIds);
	//�����洢����ʵ��
	static void BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId);
	static int charIsInstring(const wchar_t findedChar, const wchar_t* theChars);
	//ģ��ƥ��,chars1�ֶ��Ƿ���chars2��
	static bool string1_In_string2(const wchar_t* chars1, const wchar_t* chars2);
	//��ȡָ��ͼ��������ʵ��ID
	static AcDbObjectIdArray GetAllEntityId(const TCHAR* layername);
	static void SaveEntInfo(AcDbObjectIdArray& inputIds, AcGePoint2dArray& points);
	//�����ཻ��ʵ�壬ʹ�뽻������Ķ˵����óɽ����ֵ
	static void DealEnt(AcDbEntity* pEnt, AcGePoint3dArray& intersectPoints);
	//�ж������Ƿ���ͬһ��ֱ����
	static bool IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3);
	static bool IsOnLine(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3);
	static ACHAR* ChartoACHAR(const char* src);
	static char* ACHARtoChar(const ACHAR* src);
	//��ʵ����ӵ�ģ�Ϳռ���
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt); 
	//�������պ󣬽����ڶ������껻��Ϊ�䵱ǰdwg�е�����
	static void GetEntPointofinBlock();
	//�������
	static void DrowPloyLine(AcGePoint2dArray& inputpoints);
	//��ֱ��
	static AcDbObjectIdArray DrowLine(AcGePoint3dArray& inputpoints);
	//�õ���֮�����еĽ���
	static void GetAllLineIntersectpoints(AcDbObjectIdArray& inputIds, AcGePoint3dArray& outptArr);
	static void getlinepoint(AcDbObjectIdArray& lineIds, CMap<AcGePoint3d, AcGePoint3d, AcDbObjectIdArray, AcDbObjectIdArray&> &mapLines);
	//�����н����������ȡ�����¡����ϡ����ϡ����½ǵĵ�
	static void GetCornerPoint(AcGePoint3dArray& inputptArr,AcGePoint3d& lefdownPt,AcGePoint3d& lefupPt,AcGePoint3d& rigthupPt,AcGePoint3d& rigthdownPt);
	//ȷ����������㣬���ߴ��ע
	static void DrowDimaligned(const AcString& setLayerName, AcGePoint3d& point1, AcGePoint3d& point2);
	//���ߴ��ע
	static AcDbObjectId CreateDimAligned(const AcString& setLayerName, const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine,/*const AcGeVector3d& vecOffset,*/ const ACHAR* dimText);
	//�������ֱ������������һ�����λ�ã�
	static AcGePoint3d RelativePoint(const AcGePoint3d& pt, double x, double y);
	//���߶γ���
	static double GetLineLenth(AcDbObjectId& inputId);
    //ɾ��ʵ��
	static void DeleteEnt(AcDbObjectIdArray& inputIds);
	static void DeleteEnt(AcDbObjectId& inputId);
	//���߶ν���ƽ����������
	static void BatchLine(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId, double tol = 0.0);
	//�����߶��Ƿ�ƽ��,pt1��pt2Ϊһ��ֱ�����˵㣬pt3��pt4Ϊ����һ��ֱ�ߵ����˵�
	static bool IsParallel(AcGePoint3d& pt1,AcGePoint3d& pt2,AcGePoint3d& pt3, AcGePoint3d& pt4, double tol=0.0);
	//����ʵ����ɵ�����Χ��
	static AcDbExtents GetBigExtents(AcDbObjectIdArray& inputId);
	//�ж�ʵ���Ƿ���ָ��ͼ��
	static bool IsEntInLayer(AcDbObjectId& EntId, const ACHAR* layername);
	//�㵽�߶ε���̾���
	static double PointToSegDist(AcGePoint3d& linept1, AcGePoint3d& linept2, AcGePoint3d& pt);
	static double distance(AcGePoint3d& p, AcGePoint3d& p1);
	//������ѡ����ѡ���ʵ���Ƿ���ָ��ͼ��
	static bool IsIntersectEnt(const ACHAR *LineLayerName, AcGePoint3d& pt1, AcGePoint3d& pt2,AcRxClass* classDesc);
	//��ȡ��ǰͼ������������ʽ
	static void GetALLTextStyle(std::vector<CString>& textStyle);
	////�ַ����ָ����
	static void Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest);
	static void Split(const std::string& src, const std::string& separator, std::string& dest);
	//�ж��ַ����Ƿ�Ϊutf-8
	static bool is_str_utf8(const char* str);
	//�ж��ַ����Ƿ�Ϊgbk
	static bool is_str_gbk(const char* str);
	//���ⲿDWG�ļ��е���������ԵĿ���յ���ǰͼֽ��
	static bool InserBlockFormDWG(const CString& dwgblockname, const CString& dwgpath, AcGePoint3d& inserpoint,AcDbObjectId& outinserblockid);
	//��ȡָ�����ݿ���ָ������е�����ʵ���ID
	static int getIdsByDwgBlkName(AcDbDatabase *pDwg, CString strBlkName, AcDbObjectIdArray &objIds);
	static void setLayer(const CString& layerName, const int& layerColor);
	
public:
	/*
	*
	  ����������̾���
	                  *
	                   */
	static double GetLineDistance(AcDbObjectId& Line1,AcDbObjectId& Line2);
	static AcGeLineSeg2d GetGeLineObj(AcDbObjectId lineId);
	static AcGePoint2d ToPoint2d(const AcGePoint3d &point3d);

public:
	//ͨ���
	static bool PathernMatch(char *pat, char *str);

};

