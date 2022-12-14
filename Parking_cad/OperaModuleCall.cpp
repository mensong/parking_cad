#include "stdafx.h"
#include "OperaModuleCall.h"
#include "JigHelper.h"
#include "DBHelper.h"


COperaModuleCall::COperaModuleCall(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaModuleCall::~COperaModuleCall()
{
}

void COperaModuleCall::Start()
{
	ACHAR szBlkName[256];
	if (RTNORM == acedGetString(0, _T("\n请输入图块名称:"), szBlkName
#if (ACADV_RELMAJOR > 19)
		, 256
#endif
		))
	{
		mirrorJigshow(szBlkName);
	}
}

void COperaModuleCall::mirrorJigshow(const CString& blockName)
{
	AcDbObjectId  id;
	AcGePoint3d insertPt(0, 0, 0);
	AcDbObjectIdArray useJigIds;
	DBHelper::InsertBlkRef(id, blockName, insertPt);
	useJigIds.append(id);
	std::vector<AcDbEntity*> vctJigEnt;
	for (int i = 0; i < useJigIds.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		acdbOpenObject(pEnt, useJigIds[i], AcDb::kForWrite);
		//判断自定义实体的类型
		if (pEnt == NULL)
			continue;
		pEnt->setVisibility(AcDb::kInvisible);
		vctJigEnt.push_back(pEnt);
	}
	if (vctJigEnt.size() < 1)
		return;
	bool staus = true;
	CJigHelper jig;
	jig.setDispPrompt(_T("请选择放置点或者输入H、V进行水平、垂直镜像显示："));
	jig.SetBasePoint(insertPt);
	jig.RegisterAsJigEntity(vctJigEnt);
	do
	{
		CJigHelper::RESULT ec = jig.startJig();
		if (ec == CJigHelper::RET_STRING)
		{
			CString sResult = jig.GetStringResult();
			if (sResult == _T("V") || sResult == _T("v"))
			{
				AcGeVector3d verticalVec(1, 0, 0);
				AcGePoint3d verticalPt = jig.GetPosition();
				insertPt = verticalPt;
				AcGeLine3d line(verticalPt, verticalVec);
				AcGeMatrix3d mat;
				mat.setToMirroring(line);
				for (int i = 0; i < vctJigEnt.size(); ++i)
				{
					vctJigEnt[i]->transformBy(mat);
				}
			}
			else if (sResult == _T("H") || sResult == _T("h"))
			{
				AcGeVector3d levelVec(0, 1, 0);
				AcGePoint3d levelPt = jig.GetPosition();
				insertPt = levelPt;
				AcGeLine3d line(levelPt, levelVec);
				AcGeMatrix3d mat;
				mat.setToMirroring(line);
				for (int i = 0; i < vctJigEnt.size(); ++i)
				{
					vctJigEnt[i]->transformBy(mat);
				}
			}
		}
		else if (ec == CJigHelper::RET_POINT)
		{
			staus = false;
			for (int i = 0; i < vctJigEnt.size(); ++i)
			{
				vctJigEnt[i]->setVisibility(AcDb::kVisible);
				vctJigEnt[i]->close();
			}
		}
		else if (ec == CJigHelper::RET_CANCEL|| ec == CJigHelper::RET_NULL)
		{
			for (int i = 0; i < vctJigEnt.size(); ++i)
			{
				vctJigEnt[i]->erase();
				vctJigEnt[i]->close();
			}
			staus = false;
		}
	} while (staus);
	//关闭图形实体
}

REG_CMD_P(COperaModuleCall, BGY, ModuleCall);//车库精细化模块