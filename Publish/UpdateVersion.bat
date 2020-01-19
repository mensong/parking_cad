@echo off

set logfile=tmp_ver.txt

echo ============================ %~n0 ============================>%logfile%

UpdateSetupFactoryVersion -f Install.suf -s "<Filename>SetupV" -e ".exe</Filename>">ver.txt

webclient.vbs "http://127.0.0.1:8000/devops-cpp/concat-var" "%logfile%"