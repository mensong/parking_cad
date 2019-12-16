@echo off

call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad19.sln" /Build "Release|x64" >tmp19.txt

find "失败 0 个" tmp19.txt
if %errorlevel% equ 0 (
echo 正确
) else (
echo 生成有错误，请查看tmp19.txt
pause
)