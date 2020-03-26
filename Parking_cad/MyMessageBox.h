#pragma once

class MyMessageBox
{
public:
	static int Show(
		HWND hWnd,
		LPCTSTR lpText,
		LPCTSTR lpCaption,
		UINT uType,
		const std::map<int, CString>& btnText);

private:
	static LRESULT __stdcall CBTHookProc(long nCode, WPARAM wParam, LPARAM lParam);

private:
	static HHOOK ms_hHook;
	static std::map<int, CString> ms_btnText;
};

