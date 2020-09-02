::cd /d %~dp0

call readConfig.bat

start /B Clean18.bat
start /B Clean19.bat
start /B Clean20.bat
start /B Clean21.bat
start /wait Clean22.bat

rd /Q /S  ipch
rd /Q /S  .vs
::rd /Q /S  Win32
::rd /Q /S  x64
::rd /Q /S  Debug
::rd /Q /S  Release

del /F /Q /S /a %PROJECT_NAME%21.VC.db
del /F /Q /S /a %PROJECT_NAME%22.VC.db
del /F /Q /S /a %PROJECT_NAME%19.sdf
del /F /Q /S /a %PROJECT_NAME%20.sdf
del /F /Q /S /a %PROJECT_NAME%18.ncb
del /F /Q /S /a *.suo
