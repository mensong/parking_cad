@echo off

call readConfig.bat

set logfile=%CLEAN_LOG_PREX%21.log

echo ============================ %~n0 ============================>%logfile%

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "%PROJECT_NAME%21.sln" /Clean "Release|x64" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Debug|x64" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Release|x86" >>%logfile%
devenv "%PROJECT_NAME%21.sln" /Clean "Debug|x86" >>%logfile%

find "ʧ�� 1 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 2 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 3 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)


find "ʧ�� 4 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 5 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 6 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 7 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 8 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 9 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 10 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

:sucess
goto finally

:error
echo �����д�����鿴%logfile%
notepad %logfile%
goto finally

:finally
exit
