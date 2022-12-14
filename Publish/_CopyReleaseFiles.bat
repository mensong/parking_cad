@echo off
::cd /d %~dp0

rd /S /Q .\out\Support\18\x86\
rd /S /Q .\out\Support\18\x64\
rd /S /Q .\out\Support\19\x86\
rd /S /Q .\out\Support\19\x64\
rd /S /Q .\out\Support\20\x86\
rd /S /Q .\out\Support\20\x64\
rd /S /Q .\out\Support\21\x86\
rd /S /Q .\out\Support\21\x64\
rd /S /Q .\out\Support\22\x86\
rd /S /Q .\out\Support\22\x64\
rd /S /Q .\out\Support\CommonData\

mkdir .\out\Support\18\x86\
mkdir .\out\Support\18\x64\
mkdir .\out\Support\19\x86\
mkdir .\out\Support\19\x64\
mkdir .\out\Support\20\x86\
mkdir .\out\Support\20\x64\
mkdir .\out\Support\21\x86\
mkdir .\out\Support\21\x64\
mkdir .\out\Support\22\x86\
mkdir .\out\Support\22\x64\
mkdir .\out\Support\CommonData\

copy /Y /V ..\x64\Release\*.ini .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.LIN .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.bmp .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.html .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.dwg .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.json .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.cuix .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.mnr .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.mnr .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.gif .\out\Support\CommonData\
copy /Y /V ..\x64\Release\*.ico .\out\Support\CommonData\

copy /Y /V ..\x64\Release\*18.arx .\out\Support\18\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\18\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\18\x64\

copy /Y /V ..\x64\Release\*19.arx .\out\Support\19\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\19\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\19\x64\

copy /Y /V ..\x64\Release\*20.arx .\out\Support\20\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\20\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\20\x64\

copy /Y /V ..\x64\Release\*21.arx .\out\Support\21\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\21\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\21\x64\

copy /Y /V ..\x64\Release\*22.arx .\out\Support\22\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\22\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\22\x64\

