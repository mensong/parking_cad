@echo off
cd /d "%~dp0"

copy /Y /V "%MultiCadStarter%\OpenInCad.exe" ".\out\"

copy /Y /V "%MultiCadStarter%\Support\18\CadVer.exe" ".\out\Support\18\"
copy /Y /V "%MultiCadStarter%\Support\19\CadVer.exe" ".\out\Support\19\"
copy /Y /V "%MultiCadStarter%\Support\20\CadVer.exe" ".\out\Support\20\"
copy /Y /V "%MultiCadStarter%\Support\21\CadVer.exe" ".\out\Support\21\"
copy /Y /V "%MultiCadStarter%\Support\22\CadVer.exe" ".\out\Support\22\"

pause