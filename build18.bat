@echo off

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad18.sln" /Build "Release|x64" >tmp18.txt

set logfile=tmp18.txt

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
exit

:error
echo �����д�����鿴%logfile%
notepad %logfile%
