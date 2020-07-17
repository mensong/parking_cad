#pragma once
#include <tchar.h>
#include <string>
#include <process.h>

typedef struct WD_RANGE
{
	int minRange;
	int maxRange;

	WD_RANGE(int minRange = 0, int maxRange=100)
	{
		this->minRange = minRange;
		this->maxRange = maxRange;
	}
} WD_RANGE;

typedef struct WD_MSG
{
	int curPos;
	TCHAR msg[512];

	WD_MSG(int curPos=0, const TCHAR* msg=_T(""))
	{
		this->curPos = curPos;
		if (msg)
			_tcscpy_s(this->msg, msg);
		else
			_tcscpy_s(this->msg, _T(""));
	}
} WD_MSG;

typedef struct WD_SETTITLE
{
	TCHAR title[MAX_PATH];

	WD_SETTITLE(const TCHAR* title=_T(""))
	{
		if (title)
			_tcscpy_s(this->title, title);
		else
			_tcscpy_s(this->title, _T(""));
	}
} WD_SETTITLE;

typedef struct WD_RECT
{
	RECT rc;

	WD_RECT()
	{
		memset(&rc, 0, sizeof(rc));
	}

	WD_RECT(LONG left, LONG top, LONG right, LONG bottom)
	{
		rc.left = left;
		rc.top = top;
		rc.right = right;
		rc.bottom = bottom;
	}
} WD_RECT;


#define WM_WD_RESET		(WM_USER + 1)
#define WM_WD_CLOSE		(WM_USER + 2)
#define WM_WD_GETPOS	(WM_USER + 3)
#define WM_WD_GETRANGE	(WM_USER + 4)
#define WM_WD_GETID		(WM_USER + 5)
#define WM_WD_SETSHOWTIME (WM_USER + 6)


typedef struct PROCESS_READ_WRITE
{
	HANDLE hRead;
	std::string* sPrintText;
	HANDLE ev;

	PROCESS_READ_WRITE(HANDLE hRead, std::string* sPrintText, HANDLE ev)
	{
		this->hRead = hRead;
		this->sPrintText = sPrintText;
		this->ev = ev;
	}
} PROCESS_READ_WRITE;


class WD
{
public:
	//创建窗口
	static LRESULT Create(const char* exe = NULL, LONG id = NULL, RECT* pRect = NULL)
	{
		HWND h = FindWindowById(id);
		if (h)
			return S_OK;

		std::string sExePath;
		if (exe)
			sExePath = exe;
		else
			sExePath = "WaitingDialog.exe";

		char szParam[256] = { 0 };
		if (!pRect)
		{
			ltoa(id, szParam, 10);
		}
		else
		{
			sprintf(szParam, "%d %d %d %d %d", id, pRect->left, pRect->top, pRect->right, pRect->bottom);
		}
		
		unsigned long exitCode = 0;
		bool b = Execute(sExePath.c_str(), szParam, exitCode, NULL, 1000);
				
		if (b)
		{			
			return S_OK;
		}

		return -1;
	}

	//设置窗口标题
	static LRESULT SetTitle(const TCHAR* title, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		WD_SETTITLE t(title);
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = &t;
		cds.cbData = sizeof(t);
		return ::SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cds);
	}

	//设置进度范围
	static LRESULT SetRange(int minRange, int maxRange, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		WD_RANGE r(minRange, maxRange);
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = &r;
		cds.cbData = sizeof(r);
		return ::SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cds);
	}

	//消息，curPos==-1时，进度+1
	static LRESULT AppendMsg(const TCHAR* msg, int curPos = -1, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;
		
		WD_MSG m(curPos, msg);
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = &m;
		cds.cbData = sizeof(m);
		return ::SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cds);
	}

	//重置进度
	static LRESULT Reset(LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		return ::SendMessage(h, WM_WD_RESET, 0, 0);
	}

	//关闭窗口
	static LRESULT Close(LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		return ::SendMessage(h, WM_WD_CLOSE, 0, 0);
	}

	//获得当前位置
	static int GetPos(LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		return ::SendMessage(h, WM_WD_GETPOS, 0, 0);
	}

	//获得范围
	static LRESULT GetRange(int& mi, int& ma, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		mi = ::SendMessage(h, WM_WD_GETRANGE, 0, 1);
		ma = ::SendMessage(h, WM_WD_GETRANGE, 0, 2);
		return S_OK;
	}

	static BOOL ShowWindow(int nCmdShow, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return FALSE;

		return ::ShowWindow(h, nCmdShow);
	}

	static LRESULT ShowTime(BOOL show, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return -1;

		return ::SendMessage(h, WM_WD_SETSHOWTIME, 0, (LPARAM)show);
	}
	
	static BOOL MoveWindow(LONG left, LONG top, LONG right = -1, LONG bottom = -1, LONG id = NULL)
	{
		HWND h = FindWindowById(id);
		if (!h)
			return FALSE;

		if (right == 0 || bottom == 0)
		{
			RECT rcOld;
			::GetWindowRect(h, &rcOld);
			if (right == -1)
				right = left + (rcOld.right - rcOld.left);
			if (bottom == -1)
				bottom = top + (rcOld.bottom - rcOld.top);
		}
		
		WD_RECT rc(left, top, right, bottom);
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.lpData = &rc;
		cds.cbData = sizeof(rc);
		return ::SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cds);
	}

protected:
	static unsigned __stdcall _Execute_readAndWrite(void* arg)
	{
		PROCESS_READ_WRITE* tpParams = (PROCESS_READ_WRITE*)arg;
		HANDLE hRead = tpParams->hRead;
		std::string* sPrintText = tpParams->sPrintText;
		HANDLE ev = tpParams->ev;

		//读取命令行返回值
		char buff[1024 + 1];
		DWORD dwRead = 0;
		while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
		{
			if (sPrintText)
			{
				buff[dwRead] = '\0';
				sPrintText->append(buff, dwRead);
			}
		}

		SetEvent(ev);

		return 0;
	}

	static bool Execute(const char* szFile, const char* szParam, unsigned long& exitCode, std::string* sPrintText = NULL, unsigned long timeout = 0)
	{
		if (!szFile || szFile[0] == '\0')
			return false;

		HANDLE hRead, hWrite;
		//创建匿名管道
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
		if (!CreatePipe(&hRead, &hWrite, &sa, 0))
		{
			return false;
		}

		int nCmdLen = 0;
		if (szParam)
			nCmdLen = (strlen(szFile) + strlen(szParam) + 4) * sizeof(char);
		else
			nCmdLen = (strlen(szFile) + 4) * sizeof(char);
		char* szCmd = (char*)_alloca(nCmdLen);//_alloca在栈上申请的，会自动释放
		memset(szCmd, 0, nCmdLen);
		strcpy(szCmd, "\"");
		strcat(szCmd, szFile);
		strcat(szCmd, "\"");
		if (szParam)
		{
			strcat(szCmd, " ");
			strcat(szCmd, szParam);
		}

		//设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite)
		STARTUPINFOA si = { sizeof(STARTUPINFOA) };
		GetStartupInfoA(&si);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_NORMAL;
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;

		//启动命令行
		PROCESS_INFORMATION pi;
		if (!CreateProcessA(NULL, (char *)szCmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
		{
			CloseHandle(hWrite);
			CloseHandle(hRead);
			return false;
		}

		//立即关闭hWrite
		CloseHandle(hWrite);

		HANDLE ev = CreateEventA(NULL, TRUE, FALSE, NULL);

		bool bRet = true;

		unsigned int uiThreadID = 0;
		HANDLE hThreadRW = (HANDLE)_beginthreadex(NULL, 0, _Execute_readAndWrite, 
			(void*)&(PROCESS_READ_WRITE(hRead, sPrintText, ev)), 0, &uiThreadID);

		DWORD waitRet = 0;
		if (timeout > 0)
			waitRet = WaitForSingleObject(ev, timeout);
		else
			waitRet = WaitForSingleObject(ev, INFINITE);

		switch (waitRet)
		{
		case WAIT_TIMEOUT:
			bRet = true;
			break;
		case WAIT_FAILED:
			TerminateThread(hThreadRW, 1);
			TerminateProcess(pi.hProcess, 1);
			bRet = false;
			break;
		case WAIT_OBJECT_0:		
			GetExitCodeProcess(pi.hProcess, &exitCode);//获得返回值
			bRet = true;
			break;
		}

		CloseHandle(hRead);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(ev);

		return bRet;
	}

	static HWND FindWindowById(LONG id)
	{
		HWND h = NULL;
		do
		{
			h = ::FindWindowEx(NULL, h, _T("WaitingDialog"), NULL);
			if (h)
			{
				LONG _id = SendMessage(h, WM_WD_GETID, 0, 0);
				if (_id == id)
					return h;
			}
		} while(h != 0);

		return NULL;
	}
};
