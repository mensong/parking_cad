@echo off
::cd /d %~dp0

set logfile=tmp_makeInstall.txt

echo ============================ %~n0 ============================>%logfile%

SetupFactory9Trial\SUFDesign.exe Install.suf>>%logfile%

echo ============================ ȫ��������� ============================>>%logfile%

webclient.vbs "http://127.0.0.1:8000/devops-cpp/concat-var" "%logfile%"