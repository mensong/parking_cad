@echo off

call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad20.sln" /Build "Release|x64" >tmp20.txt

find "ʧ�� 0 ��" tmp20.txt
if %errorlevel% equ 0 (
echo ��ȷ
) else (
echo �����д�����鿴tmp20.txt
pause
)