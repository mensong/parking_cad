@echo off

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad21.sln" /Build "Release|x64" >tmp21.txt

find "失败 0 个" tmp21.txt
if %errorlevel% equ 0 (
echo 正确
) else (
echo 生成有错误，请查看tmp21.txt
pause
)