#include "stdafx.h"
#include "OperaAddFrame.h"
#include "DBHelper.h"
#include "DlgAddFrame.h"

COperaAddFrame::COperaAddFrame()
{
}

COperaAddFrame::~COperaAddFrame()
{
}

void COperaAddFrame::Start()
{
	std::vector<AcDbObjectId> ids = DBHelper::SelectEntityPrompt(_T("\n选择要生成图框的实体:"));
	if (ids.size() < 1)
		return;

	AcDbExtents extFrame;
	for (int i=0; i<ids.size(); ++i)
	{
		AcDbExtents ext;
		Acad::ErrorStatus es = DBHelper::GetEntityExtents(ext, ids[i]);
		if (Acad::eOk == es)
			extFrame.addExt(ext);
	}

	CAcModuleResourceOverride resOverride;//资源定位
	CDlgAddFrame dlg;
	dlg.setContextExtents(extFrame);
	INT_PTR res = dlg.DoModal();
	if (res == IDOK)
	{
		dlg.setBigFramePoints();

		std::string Textstr = "SP=4865|SPT=3740|SPF=1125|SPF1=210|SPF2=450|SPF3=463|CP=132|JSPC=25|SPC=33|H=3.55|HT=1";//这里为测试，具体数据格式还需后续与客户确认
		dlg.setBlockInserPoint(Textstr);

		AcDbPolyline* pFrame = new AcDbPolyline;
		pFrame->addVertexAt(0, dlg.mBigFramept0);
		pFrame->addVertexAt(1, dlg.mBigFramept1);
		pFrame->addVertexAt(2, dlg.mBigFramept2);
		pFrame->addVertexAt(3, dlg.mBigFramept3);
		pFrame->setClosed(Adesk::kTrue);
		pFrame->setColorIndex(255);

		AcGePoint2d centerpt = AcGePoint2d((dlg.mBigFramept0.x + dlg.mBigFramept2.x) / 2, (dlg.mBigFramept0.y+ dlg.mBigFramept2.y) / 2);

		AcDbPolyline* pOutermostFrame = new AcDbPolyline;
		pOutermostFrame->addVertexAt(0, GetChangePoint(centerpt, dlg.mBigFramept0));
		pOutermostFrame->addVertexAt(1, GetChangePoint(centerpt, dlg.mBigFramept1));
		pOutermostFrame->addVertexAt(2, GetChangePoint(centerpt, dlg.mBigFramept2));
		pOutermostFrame->addVertexAt(3, GetChangePoint(centerpt, dlg.mBigFramept3));
		pOutermostFrame->setClosed(Adesk::kTrue);
		pOutermostFrame->setColorIndex(255);

		std::vector<AcDbEntity*> vcEnts;
		vcEnts.push_back(pFrame);
		vcEnts.push_back(pOutermostFrame);
		DBHelper::CreateBlock(_T("图框"), vcEnts);
		if(pFrame)
		pFrame->close();
		if(pOutermostFrame)
		pOutermostFrame->close();

		AcDbObjectId idEnt;
		DBHelper::InsertBlkRef(idEnt, _T("图框"), AcGePoint3d(0, 0, 0));

	}
}

AcGePoint2d COperaAddFrame::GetChangePoint(AcGePoint2d& centerpt, AcGePoint2d& changpt)
{
	AcGeVector2d vec(changpt - centerpt);
	vec.normalize();
	AcGePoint2d pt = changpt;
	pt.transformBy(vec * 4000);
	return pt;
}


REG_CMD(COperaAddFrame, BGY, AddFrame);//增加图框