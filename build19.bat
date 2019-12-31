@echo off

set logfile=tmp19.txt

call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad19.sln" /Build "Release|x64" >%logfile%

webclient.vbs "http://127.0.0.1:8000/devops-cpp/concat-var" "%logfile%"

find "失败 1 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 2 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 3 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 4 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 5 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 6 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 7 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 8 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 9 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 10 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

:sucess
exit

:error
echo 生成有错误，请查看%logfile%
::notepad %logfile%
webclient.vbs "http://127.0.0.1:8000/devops-cpp/set-var?status=3"
exit
