@echo off
SETLOCAL ENABLEEXTENSIONS

::ArxTools
if not DEFINED ArxTools ( 
	echo ���Ȱ�װArxTools
	goto :end 
)
copy /Y /V "%ArxTools%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%ArxTools%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%ArxTools%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%ArxTools%\..\Win32\Release\*.*" ".\Win32\Release\"

::KV
if not DEFINED KV ( 
	echo ���Ȱ�װKV
	goto :end 
)
copy /Y /V "%KV%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%KV%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%KV%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%KV%\..\Win32\Release\*.*" ".\Win32\Release\"

::L09
if not DEFINED L09 ( 
	echo ���Ȱ�װL09
	goto :end 
)
copy /Y /V "%L09%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%L09%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%L09%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%L09%\..\Win32\Release\*.*" ".\Win32\Release\"

::lib_bipsignin
if not DEFINED lib_bipsignin ( 
	echo ���Ȱ�װlib_bipsignin
	goto :end 
)
copy /Y /V "%lib_bipsignin%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%lib_bipsignin%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%lib_bipsignin%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%lib_bipsignin%\..\Win32\Release\*.*" ".\Win32\Release\"

::LibcurlHttp
if not DEFINED LibcurlHttp ( 
	echo ���Ȱ�װLibcurlHttp
	goto :end 
)
copy /Y /V "%LibcurlHttp%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%LibcurlHttp%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%LibcurlHttp%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%LibcurlHttp%\..\Win32\Release\*.*" ".\Win32\Release\"

::WD
if not DEFINED WD ( 
	echo ���Ȱ�װWD
	goto :end 
)
copy /Y /V "%WD%\..\bin\*.*" ".\x64\Debug\"
copy /Y /V "%WD%\..\bin\*.*" ".\x64\Release\"
copy /Y /V "%WD%\..\bin\*.*" ".\Win32\Debug\"
copy /Y /V "%WD%\..\bin\*.*" ".\Win32\Release\"


:end
