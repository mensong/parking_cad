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
	//分批存储相连实体
	static void BatchStorageEnt(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId);
	static int charIsInstring(const wchar_t findedChar, const wchar_t* theChars);
	//模糊匹配,chars1字段是否在chars2中
	static bool string1_In_string2(const wchar_t* chars1, const wchar_t* chars2);
	//获取指定图层上所有实体ID
	static AcDbObjectIdArray GetAllEntityId(const TCHAR* layername);
	static void SaveEntInfo(AcDbObjectIdArray& inputIds, AcGePoint2dArray& points);
	//处理相交的实体，使离交点最近的端点设置成交点的值
	static void DealEnt(AcDbEntity* pEnt, AcGePoint3dArray& intersectPoints);
	//判断三点是否在同一条直线上
	static bool IsOnLine(AcGePoint2d& pt1, AcGePoint2d& pt2, AcGePoint2d& pt3);
	static bool IsOnLine(AcGePoint3d& pt1, AcGePoint3d& pt2, AcGePoint3d& pt3);
	static ACHAR* ChartoACHAR(const char* src);
	static char* ACHARtoChar(const ACHAR* src);
	//将实体添加到模型空间中
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt); 
	//插入块参照后，将块内对象坐标换算为其当前dwg中的坐标
	static void GetEntPointofinBlock();
	//画多段线
	static void DrowPloyLine(AcGePoint2dArray& inputpoints);
	//画直线
	static AcDbObjectIdArray DrowLine(AcGePoint3dArray& inputpoints);
	//得到线之间所有的交点
	static void GetAllLineIntersectpoints(AcDbObjectIdArray& inputIds, AcGePoint3dArray& outptArr);
	static void getlinepoint(AcDbObjectIdArray& lineIds, CMap<AcGePoint3d, AcGePoint3d, AcDbObjectIdArray, AcDbObjectIdArray&> &mapLines);
	//将所有交点进行排序，取得左下、左上、右上、右下角的点
	static void GetCornerPoint(AcGePoint3dArray& inputptArr,AcGePoint3d& lefdownPt,AcGePoint3d& lefupPt,AcGePoint3d& rigthupPt,AcGePoint3d& rigthdownPt);
	//确定方向和两点，画尺寸标注
	static void DrowDimaligned(const AcString& setLayerName, AcGePoint3d& point1, AcGePoint3d& point2);
	//画尺寸标注
	static AcDbObjectId CreateDimAligned(const AcString& setLayerName, const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& ptLine,/*const AcGeVector3d& vecOffset,*/ const ACHAR* dimText);
	//根据相对直角坐标来计算一个点的位置：
	static AcGePoint3d RelativePoint(const AcGePoint3d& pt, double x, double y);
	//求线段长度
	static double GetLineLenth(AcDbObjectId& inputId);
    //删除实体
	static void DeleteEnt(AcDbObjectIdArray& inputIds);
	static void DeleteEnt(AcDbObjectId& inputId);
	//将线段进行平行批量分组
	static void BatchLine(AcDbObjectIdArray& inputId, std::vector<std::vector<AcDbObjectId>>& outputId, double tol = 0.0);
	//两条线段是否平行,pt1和pt2为一条直线两端点，pt3和pt4为另外一条直线的两端点
	static bool IsParallel(AcGePoint3d& pt1,AcGePoint3d& pt2,AcGePoint3d& pt3, AcGePoint3d& pt4, double tol=0.0);
	//求多个实体组成的最大包围框
	static AcDbExtents GetBigExtents(AcDbObjectIdArray& inputId);
	//判断实体是否在指定图层
	static bool IsEntInLayer(AcDbObjectId& EntId, const ACHAR* layername);
	//点到线段的最短距离
	static double PointToSegDist(AcGePoint3d& linept1, AcGePoint3d& linept2, AcGePoint3d& pt);
	static double distance(AcGePoint3d& p, AcGePoint3d& p1);
	//利用线选，所选择的实体是否在指定图层
	static bool IsIntersectEnt(const ACHAR *LineLayerName, AcGePoint3d& pt1, AcGePoint3d& pt2,AcRxClass* classDesc);
	//获取当前图层所有文字样式
	static void GetALLTextStyle(std::vector<CString>& textStyle);
	////字符串分割到数组
	static void Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest);
	static void Split(const std::string& src, const std::string& separator, std::string& dest);
	//判断字符串是否为utf-8
	static bool is_str_utf8(const char* str);
	//判断字符串是否为gbk
	static bool is_str_gbk(const char* str);
	//从外部DWG文件中导入带有属性的块参照到当前图纸中
	static bool InserBlockFormDWG(const CString& dwgblockname, const CString& dwgpath, AcGePoint3d& inserpoint,AcDbObjectId& outinserblockid);
	//获取指定数据库中指定块表中的所有实体的ID
	static int getIdsByDwgBlkName(AcDbDatabase *pDwg, CString strBlkName, AcDbObjectIdArray &objIds);
	static void setLayer(const CString& layerName, const int& layerColor);
	
public:
	/*
	*
	  两条曲线最短距离
	                  *
	                   */
	static double GetLineDistance(AcDbObjectId& Line1,AcDbObjectId& Line2);
	static AcGeLineSeg2d GetGeLineObj(AcDbObjectId lineId);
	static AcGePoint2d ToPoint2d(const AcGePoint3d &point3d);

public:
	//通配符
	static bool PathernMatch(char *pat, char *str);

};

