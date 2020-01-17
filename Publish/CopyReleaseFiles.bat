@echo off

set logfile=tmp_copyFile.txt

echo ============================ %~n0 ============================>%logfile%

call _CopyReleaseFiles.bat>>"%logfile%"

webclient.vbs "http://127.0.0.1:8000/devops-cpp/concat-var" "%logfile%"