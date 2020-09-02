@echo off

call readConfig.bat

set logfile=%CLEAN_LOG_PREX%21.log

echo ============================ %~n0 ============================>%logfile%

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "%PROJECT_NAME%21.sln" /Clean "Release|x64" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Debug|x64" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Release|x86" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Debug|x86" >>%logfile%

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
goto finally

:error
echo 生成有错误，请查看%logfile%
notepad %logfile%
goto finally

:finally
exit
