@echo off

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad21.sln" /Build "Release|x64" >tmp21.txt

set logfile=tmp21.txt

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
notepad %logfile%
