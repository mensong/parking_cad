@echo off
SETLOCAL ENABLEEXTENSIONS

::ArxTools
if not DEFINED ArxTools ( 
	echo 请先安装ArxTools
	goto :end 
)
copy /Y /V "%ArxTools%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%ArxTools%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%ArxTools%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%ArxTools%\..\Win32\Release\*.*" ".\Win32\Release\"

::KV
if not DEFINED KV ( 
	echo 请先安装KV
	goto :end 
)
copy /Y /V "%KV%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%KV%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%KV%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%KV%\..\Win32\Release\*.*" ".\Win32\Release\"

::L09
if not DEFINED L09 ( 
	echo 请先安装L09
	goto :end 
)
copy /Y /V "%L09%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%L09%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%L09%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%L09%\..\Win32\Release\*.*" ".\Win32\Release\"

::lib_bipsignin
if not DEFINED lib_bipsignin ( 
	echo 请先安装lib_bipsignin
	goto :end 
)
copy /Y /V "%lib_bipsignin%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%lib_bipsignin%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%lib_bipsignin%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%lib_bipsignin%\..\Win32\Release\*.*" ".\Win32\Release\"

::LibcurlHttp
if not DEFINED LibcurlHttp ( 
	echo 请先安装LibcurlHttp
	goto :end 
)
copy /Y /V "%LibcurlHttp%\..\x64\Debug\*.*" ".\x64\Debug\"
copy /Y /V "%LibcurlHttp%\..\x64\Release\*.*" ".\x64\Release\"
copy /Y /V "%LibcurlHttp%\..\Win32\Debug\*.*" ".\Win32\Debug\"
copy /Y /V "%LibcurlHttp%\..\Win32\Release\*.*" ".\Win32\Release\"

::WD
if not DEFINED WD ( 
	echo 请先安装WD
	goto :end 
)
copy /Y /V "%WD%\..\bin\*.*" ".\x64\Debug\"
copy /Y /V "%WD%\..\bin\*.*" ".\x64\Release\"
copy /Y /V "%WD%\..\bin\*.*" ".\Win32\Debug\"
copy /Y /V "%WD%\..\bin\*.*" ".\Win32\Release\"


:end
