#pragma once
#include "HackARX.h"

class HackerRectStair :
	public IDbDwgFiler
{
public:
	HackerRectStair(AcDbObject* pObj);
	~HackerRectStair(void);

	static AcString Desc();

	AcString ToString();

protected:
	virtual Acad::ErrorStatus writeDouble(int evalCount, double);
	virtual Acad::ErrorStatus writePoint3d(int evalCount, const AcGePoint3d& pt);
	virtual Acad::ErrorStatus writeUInt8(int evalCount, Adesk::UInt8);
	virtual Acad::ErrorStatus writeInt32(int evalCount, Adesk::Int32);
	virtual Acad::ErrorStatus writeUInt32(int evalCount, Adesk::UInt32);
	virtual Acad::ErrorStatus writeUInt16(int evalCount, Adesk::UInt16);

protected:
	int _getString2Offset();
	int _getInt16Offset();

private:
	AcGePoint3d m_position;//位置
	double m_rotation;//梯段旋转
	double m_width;//梯间宽度
	double m_singleWidth;//梯段宽度
	Adesk::UInt8 m_step1Count;//一跑步数
	Adesk::UInt8 m_step2Count;//二跑步数
	double stepHeight;//踏步高度
	double stepWidth;//踏步宽度
	double handrailWidth;//扶手宽度
	double handrailSideDis;//扶手边距

	/*这里包含了：“有/无内侧栏杆”“有无外侧扶手”“是/否扶手连接”
	“平台类型:矩形平台/弧形平台”“有/无休息平台”“有/无内边梁”“有/无外边梁”
	“是/否作为坡道”“是/否有防滑条”“是/否落地”“是/否有疏散半径”“疏散半径单/双侧”
	*/
	Adesk::UInt32 handrailsParts;//楼梯类型

	double rectHandrailExtende;//转角扶手伸出
	double startAndEndHandrailExtende;//层间扶手伸出

	double restingPlatformWidth;//平台宽度
	double restingPlatformThickness;//平台厚度

	Adesk::UInt8 stepAlignmentType;//踏步取齐

	bool leftUpstairs;//是否是左边上楼

	Adesk::UInt8 floorFeatures;//楼层标记

	Adesk::Int32 m_customGoUpstairsText;//1:文字全是用默认；2:自定义了上楼文字或下楼文字；3:同时自定义了上、下楼文字；

	Adesk::UInt16 evacuationDetectionLine;//疏散半径颜色

};

