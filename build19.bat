call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad19.sln" /Build "Release|x64"
pause