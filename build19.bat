@echo off

call readConfig.bat

set logfile=%BUILD_LOG_PREX%19.log

echo ============================ %~n0 ============================>%logfile%

call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "%PROJECT_NAME%19.sln" /Build "Release|x64" >>%logfile%

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
echo ============================ ���ɴ��� ============================>>%logfile%
::webclient.vbs "http://127.0.0.1:8000/devops-cpp/set-var?status=3"
goto finally

:finally
::webclient.vbs "http://127.0.0.1:8000/devops-cpp/concat-var" "%logfile%"

exit
