@echo off

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad22.sln" /Build "Release|x64" >tmp22.txt

find "ʧ�� 0 ��" tmp22.txt
if %errorlevel% equ 0 (
echo ��ȷ
) else (
echo �����д�����鿴tmp22.txt
pause
)