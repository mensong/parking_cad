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
	std::vector<AcDbObjectId> ids = DBHelper::SelectEntityPrompt(_T("\nѡ��Ҫ����ͼ���ʵ��:"));
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

	CAcModuleResourceOverride resOverride;//��Դ��λ
	CDlgAddFrame dlg;
	dlg.setContextExtents(extFrame);
	INT_PTR res = dlg.DoModal();
	if (res == IDOK)
	{

	}
}

REG_CMD(COperaAddFrame, BGY, AddFrame);