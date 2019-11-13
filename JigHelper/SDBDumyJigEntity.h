/**
 * @file    SDBDumyJigEntity.h
 * @brief   Jig����ʵ����
 * @author  Gergul
 * @date    2018/10/29
 * Note:    ǿ�Ƶ���ͼ��ʵ���worldDraw��viewportDraw�����ػ�ͼ��ʵ��
 */
#pragma once
#include "dbents.h"

/**
 * @class   CSDBDumyJigEntity
 * @brief   Jig����ʵ����
 * @author  Gergul
 * @date    2018/10/29
 * Note:    
 */
class CSDBDumyJigEntity : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(CSDBDumyJigEntity);

	CSDBDumyJigEntity(void);

	virtual ~CSDBDumyJigEntity(void);

public:
	//ͨ��ָ�����ʵ��
	void addEntity(AcDbEntity* pEnt);
	//��ȡʵ��
	AcDbEntity* getEnity(int pos) const;
	//���ʵ�����
	int getEntityCount();
	//ɾ��ʵ��
	bool remove(AcDbEntity *pEntRemove);
	bool remove(int nIdex);
	//���ʵ��
	bool removeAll();

public:
	/** ����������ƽӿ�
	 *	@param   mode - �ӿ��������ָ��
	 */
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode);
	/** ��һ�ӿڻ��ƽӿ�
	 *	@param   mode - �ӿ��������ָ��
	 */
	virtual void subViewportDraw(AcGiViewportDraw* pVd);

public:
	//�ⲿopen��ʵ��
	AcDbVoidPtrArray m_entArray;
	//����viewportDraw�Ķ���
	AcDbVoidPtrArray m_entArrayViewPointDraw;
};

//#ifdef SDENTITY_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(CSDBDumyJigEntity)
	//acrxClassDictionary
//#endif