#pragma once

typedef struct ParkingLogContext
{
	CString type;
	int error;
	CString descr;
	int trigger_count;
	CString user_udid;

} ParkingLogContext;

static void* MakeLogMessage(const CString& type, int error, const CString& descr,
	int trigger_count = 1, CString& user_udid = CString())
{
	ParkingLogContext* pContext = new ParkingLogContext;
	pContext->type = type;
	pContext->error = error;
	pContext->descr = descr;
	pContext->trigger_count = trigger_count;
	pContext->user_udid = user_udid;
	return (void*)pContext;
}