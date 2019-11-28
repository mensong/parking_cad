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
	AcGePoint3d m_position;//λ��
	double m_rotation;//�ݶ���ת
	double m_width;//�ݼ���
	double m_singleWidth;//�ݶο��
	Adesk::UInt8 m_step1Count;//һ�ܲ���
	Adesk::UInt8 m_step2Count;//���ܲ���
	double stepHeight;//̤���߶�
	double stepWidth;//̤�����
	double handrailWidth;//���ֿ��
	double handrailSideDis;//���ֱ߾�

	/*��������ˣ�����/���ڲ����ˡ������������֡�����/��������ӡ�
	��ƽ̨����:����ƽ̨/����ƽ̨������/����Ϣƽ̨������/���ڱ���������/���������
	����/����Ϊ�µ�������/���з�����������/����ء�����/������ɢ�뾶������ɢ�뾶��/˫�ࡱ
	*/
	Adesk::UInt32 handrailsParts;//¥������

	double rectHandrailExtende;//ת�Ƿ������
	double startAndEndHandrailExtende;//���������

	double restingPlatformWidth;//ƽ̨���
	double restingPlatformThickness;//ƽ̨���

	Adesk::UInt8 stepAlignmentType;//̤��ȡ��

	bool leftUpstairs;//�Ƿ��������¥

	Adesk::UInt8 floorFeatures;//¥����

	Adesk::Int32 m_customGoUpstairsText;//1:����ȫ����Ĭ�ϣ�2:�Զ�������¥���ֻ���¥���֣�3:ͬʱ�Զ������ϡ���¥���֣�

	Adesk::UInt16 evacuationDetectionLine;//��ɢ�뾶��ɫ

};

