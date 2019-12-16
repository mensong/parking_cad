@echo off

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad18.sln" /Build "Release|x64" >tmp18.txt

find "失败 0 个" tmp18.txt
if %errorlevel% equ 0 (
echo 正确
) else (
echo 生成有错误，请查看tmp18.txt
pause
)