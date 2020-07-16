cd /d "%~dp0"

set targetDir="%AppData%\碧桂园车库智能设计系统"
rd /S /Q %targetDir%
mkdir %targetDir%
mkdir %targetDir%
CopyFiles.exe ".\CommonData" %targetDir%
