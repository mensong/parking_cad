@echo off
::cd /d %~dp0

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
copy /Y /V ..\x64\Release\NoPicture.bmp .\out\Support\CommonData\
copy /Y /V ..\x64\Release\Notice.html .\out\Support\CommonData\
copy /Y /V ..\x64\Release\template.dwg .\out\Support\CommonData\
copy /Y /V ..\x64\Release\ParkingConfig.json .\out\Support\CommonData\
copy /Y /V ..\x64\Release\Mapsign.dwg .\out\Support\CommonData\
copy /Y /V ..\x64\Release\parking_cad.cuix .\out\Support\CommonData\
copy /Y /V ..\x64\Release\parking_cad.mnr .\out\Support\CommonData\
copy /Y /V ..\x64\Release\parking_cad_light.mnr .\out\Support\CommonData\

copy /Y /V ..\x64\Release\loading.gif .\out\Support\18\x64\
copy /Y /V ..\x64\Release\exchange.ico .\out\Support\18\x64\
copy /Y /V ..\x64\Release\*18.arx .\out\Support\18\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\18\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\18\x64\

copy /Y /V ..\x64\Release\loading.gif .\out\Support\19\x64\
copy /Y /V ..\x64\Release\exchange.ico .\out\Support\19\x64\
copy /Y /V ..\x64\Release\*19.arx .\out\Support\19\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\19\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\19\x64\

copy /Y /V ..\x64\Release\loading.gif .\out\Support\20\x64\
copy /Y /V ..\x64\Release\exchange.ico .\out\Support\20\x64\
copy /Y /V ..\x64\Release\*20.arx .\out\Support\20\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\20\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\20\x64\

copy /Y /V ..\x64\Release\loading.gif .\out\Support\21\x64\
copy /Y /V ..\x64\Release\exchange.ico .\out\Support\21\x64\
copy /Y /V ..\x64\Release\*21.arx .\out\Support\21\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\21\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\21\x64\

copy /Y /V ..\x64\Release\loading.gif .\out\Support\22\x64\
copy /Y /V ..\x64\Release\exchange.ico .\out\Support\22\x64\
copy /Y /V ..\x64\Release\*22.arx .\out\Support\22\x64\
copy /Y /V ..\x64\Release\*.dll .\out\Support\22\x64\
copy /Y /V ..\x64\Release\*.exe .\out\Support\22\x64\

