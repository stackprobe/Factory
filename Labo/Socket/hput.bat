rem usage: hput URL LOCAL-FILE [PASSWORD [PROXY-DOMAIN PROXY-PORT]]
rem usage: hput URL * [PASSWORD [PROXY-DOMAIN PROXY-PORT]]
IF "%2" == "" GOTO END
DEL _content.tmp
IF "%3" == "" (
	%~dp0mmpc.exe /S upload upload /F file %2 _content.tmp
) ELSE (
	%~dp0mmpc.exe /S upload upload /S password %3 /F file %2 _content.tmp
)
IF NOT EXIST _content.tmp GOTO END
IF "%5" == "" (
	%~dp0hget.exe /C _content.tmp /O * %1
) ELSE (
	%~dp0hget.exe /PS %4 /PP %5 /C _content.tmp /O * %1
)
DEL _content.tmp
:END
