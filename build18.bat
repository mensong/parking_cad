@echo off

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad18.sln" /Build "Release|x64" >tmp18.txt

find "ʧ�� 0 ��" tmp18.txt
if %errorlevel% equ 0 (
echo ��ȷ
) else (
echo �����д�����鿴tmp18.txt
pause
)