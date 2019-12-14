call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" amd64
devenv "Parking_cad18.sln" /Build "Release|x64"
pause