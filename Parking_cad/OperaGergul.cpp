#include "stdafx.h"
#include "OperaGergul.h"
#include "GeHelper.h"
#include "ParkingLog.h"

COperaGergul::COperaGergul(const AcString& group, const AcString& cmd, const AcString& alias, Adesk::Int32 cmdFlag)
	: CIOperaLog(group, cmd, alias, cmdFlag)
{
}


COperaGergul::~COperaGergul(void)
{
}

void COperaGergul::Start()
{
	CParkingLog::AddLogA("test", 0, "√Ë ˆ");

}

REG_CMD_P(COperaGergul, BGY, Gergul);