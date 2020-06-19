#pragma once
#include "IOperaLog.h"
#include "DlgSetConfig.h"

class COperaSetConfig :
	public CIOperaLog
{
public:
	COperaSetConfig(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag);
	~COperaSetConfig();
	virtual void Start();
	AcGeTol m_tol;

	//获取acad.lin文件里的线型名称
	static std::vector<CString> getACADLINtag();
	static Acad::ErrorStatus getLineTypeId(AcDbObjectId &linetypeid, const AcString& tag, const AcString& acadlinfilepath, AcDbDatabase *pDb = acdbCurDwg());
	static Acad::ErrorStatus getLinetypeIdFromString(const AcString& tag, AcDbObjectId& id, AcDbDatabase *pDb = acdbCurDwg());
	static void loadAllLinetype();
};

