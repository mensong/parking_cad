#include "StdAfx.h"
#include "SDBDumyJigEntity.h"

//-----------------------------------------------------------------------------
//ACRX_DXF_DEFINE_MEMBERS(
//	CSDBDumyJigEntity, AcDbArc,
//	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
//	AcDbProxyEntity::kNoOperation, BDumyJigEntity,
//	BDumyJigEntityAPP
//	| Product Desc : A description for your object
//	| Company : Gergul
//	| WEB Address : Your company WEB site address
//)

AcRxClass* CSDBDumyJigEntity::desc()
{
	if (CSDBDumyJigEntity::gpDesc != NULL) 
		return CSDBDumyJigEntity::gpDesc; 
	return CSDBDumyJigEntity::gpDesc 
		= (AcRxClass*)((AcRxDictionary*)acrxSysRegistry() 
		->at(ACRX_CLASS_DICTIONARY))->at(ACRX_T("CSDBDumyJigEntity")); 
}

AcRxClass* CSDBDumyJigEntity::isA() const 
{ 
	if (CSDBDumyJigEntity::gpDesc != NULL) 
		return CSDBDumyJigEntity::gpDesc; 
	return CSDBDumyJigEntity::gpDesc 
		= (AcRxClass*)((AcRxDictionary*)acrxSysRegistry() 
		->at(ACRX_CLASS_DICTIONARY))->at(ACRX_T("CSDBDumyJigEntity")); 
} 
AcRxClass* CSDBDumyJigEntity::gpDesc = NULL;

static AcRxObject * makeCSDBDumyJigEntity() { return new CSDBDumyJigEntity(); }
void CSDBDumyJigEntity::rxInit() 
{
	AcRxClass *pClass =
		(AcRxClass*)((AcRxDictionary*)acrxSysRegistry()
		->at(ACRX_CLASS_DICTIONARY))->at(ACRX_T("CSDBDumyJigEntity"));
	if (pClass)
	{//�Ѿ�ע�������ֹ�ظ�ע������ı���
		return;
	}

	ACRX_STATIC_CHECK(CSDBDumyJigEntity);
	
	CSDBDumyJigEntity::gpDesc = newAcRxClass(ACRX_T("CSDBDumyJigEntity"), ACRX_T("AcDbEntity"),
		AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, AcDbProxyEntity::kNoOperation,
		&makeCSDBDumyJigEntity, ACRX_T("BDumyJigEntity"), ACRX_T("Gergul"));
} 
void CSDBDumyJigEntity::rxInit(AppNameChangeFuncPtr ptr) 
{
	AcRxClass *pClass =
		(AcRxClass*)((AcRxDictionary*)acrxSysRegistry()
		->at(ACRX_CLASS_DICTIONARY))->at(ACRX_T("CSDBDumyJigEntity"));
	if (pClass)
	{//�Ѿ�ע�������ֹ�ظ�ע������ı���
		return;
	}

	ACRX_STATIC_CHECK(CSDBDumyJigEntity); 
	CSDBDumyJigEntity::gpDesc = newAcRxClass(ACRX_T("CSDBDumyJigEntity"), ACRX_T("AcDbEntity"), 
		AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, AcDbProxyEntity::kNoOperation, 
		&makeCSDBDumyJigEntity, ACRX_T("BDumyJigEntity"), ACRX_T("Gergul"), ptr); \
}

//-----------------------------------------------------------------------------

CSDBDumyJigEntity::CSDBDumyJigEntity(void) :AcDbEntity()
{
}


CSDBDumyJigEntity::~CSDBDumyJigEntity(void)
{

}

//�ʺ�δ��ӵ����ݿ��ʵ��
void CSDBDumyJigEntity::addEntity(AcDbEntity* pEnt)
{
	if (pEnt != NULL)
	{
		m_entArray.append(pEnt);
	}
}


AcDbEntity* CSDBDumyJigEntity::getEnity(int pos) const
{
	return (AcDbEntity*)m_entArray[pos];
}

Adesk::Boolean CSDBDumyJigEntity::subWorldDraw(AcGiWorldDraw *mode)
{
	assertReadEnabled();
	m_entArrayViewPointDraw.removeAll();

	int n = m_entArray.length();
	for (int i = 0; i < n; i++)
	{
		AcDbEntity* pEnt = (AcDbEntity*)m_entArray[i];
		//ǿ�Ƶ���worldDraw
		if (pEnt->worldDraw(mode) == Adesk::kFalse)
		{
			m_entArrayViewPointDraw.append(pEnt);
		}
	}

	if (m_entArrayViewPointDraw.length() != 0)
	{//������Ҫʹ��ViewportDraw���Ƶ�ʵ�壬��ʹ�÷���false�Զ�subViewportDraw()
		return Adesk::kFalse;
	}

	return Adesk::kTrue;
}

void CSDBDumyJigEntity::subViewportDraw(AcGiViewportDraw* pVd)
{
	assertReadEnabled();
	int n = m_entArrayViewPointDraw.length();
	for (int i = 0; i < n; i++)
	{
		AcDbEntity* pEnt = (AcDbEntity*)m_entArrayViewPointDraw[i];
		//ǿ�Ƶ���worldDraw
		pEnt->viewportDraw(pVd);
	}
}

int CSDBDumyJigEntity::getEntityCount()
{
	return m_entArray.length();
}

bool CSDBDumyJigEntity::remove(AcDbEntity *pEntRemove)
{
	return m_entArray.remove((void *)pEntRemove);
}

bool CSDBDumyJigEntity::remove(int nIdex)
{
	m_entArray.removeAt(nIdex);
	return true;
}

bool CSDBDumyJigEntity::removeAll()
{
	m_entArray.removeAll();
	return true;
}
