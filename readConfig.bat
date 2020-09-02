::::读取与此bat相同目录下的config.txt配置文件，文件内容如下::::
:: PROJECT_NAME=Parking_cad
:: BUILD_LOG_PREX=build
:: CLEAN_LOG_PREX=clean
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set config_file=%~dp0\config.txt
::echo %config_file%
for /f "tokens=1,2 delims==" %%i in (%config_file%) do (
	set %%i=%%j
)
