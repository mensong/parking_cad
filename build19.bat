@echo off

call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad19.sln" /Build "Release|x64" >tmp19.txt

find "ʧ�� 0 ��" tmp19.txt
if %errorlevel% equ 0 (
echo ��ȷ
) else (
echo �����д�����鿴tmp19.txt
pause
)