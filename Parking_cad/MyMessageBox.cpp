#include "stdafx.h"
#include "MyMessageBox.h"

HHOOK MyMessageBox::ms_hHook;
std::map<int, CString> MyMessageBox::ms_btnText;

int MyMessageBox::Show(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, const std::map<int, CString>& btnText)
{
	ms_btnText = btnText;
	ms_hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc, NULL, GetCurrentThreadId());
	return ::MessageBox(hWnd, lpText, lpCaption, uType);
}

LRESULT __stdcall MyMessageBox::CBTHookProc(long nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_ACTIVATE)
	{
		HWND hWnd = (HWND)wParam;
		for (std::map<int, CString>::iterator it = ms_btnText.begin(); it != ms_btnText.end(); ++it)
		{
			SetDlgItemText(hWnd, it->first, it->second);
		}
		ms_btnText.clear();
		UnhookWindowsHookEx(ms_hHook);
	}
	return 0;
}


