@echo off

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad21.sln" /Build "Release|x64" >tmp21.txt

find "ʧ�� 0 ��" tmp21.txt
if %errorlevel% equ 0 (
echo ��ȷ
) else (
echo �����д�����鿴tmp21.txt
pause
)