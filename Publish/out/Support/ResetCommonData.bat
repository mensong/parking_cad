cd /d "%~dp0"

set targetDir="%AppData%\�̹�԰�����������ϵͳ"
rd /S /Q %targetDir%
mkdir %targetDir%
mkdir %targetDir%
CopyFiles.exe ".\CommonData" %targetDir%
