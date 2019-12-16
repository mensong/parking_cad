@echo off

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad22.sln" /Build "Release|x64" >tmp22.txt

find "失败 0 个" tmp22.txt
if %errorlevel% equ 0 (
echo 正确
) else (
echo 生成有错误，请查看tmp22.txt
pause
)