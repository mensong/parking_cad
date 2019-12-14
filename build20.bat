call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad20.sln" /Build "Release|x64"
pause