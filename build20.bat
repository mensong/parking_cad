@echo off

call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad20.sln" /Build "Release|x64" >tmp20.txt

find "失败 0 个" tmp20.txt
if %errorlevel% equ 0 (
echo 正确
) else (
echo 生成有错误，请查看tmp20.txt
pause
)