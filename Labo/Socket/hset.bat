rem usage: hset URL LOCAL-FILE [PASSWORD [PROXY-DOMAIN PROXY-PORT]]
IF "%2" == "" GOTO END
IF "%3" == "" (
	%~dp0mmpc.exe /S delete %~nx2 _content.tmp
	%~dp0mmpc.exe /S delete %~nx2.clu _content2.tmp
) ELSE (
	%~dp0mmpc.exe /S delete %~nx2 /S password %3 _content.tmp
	%~dp0mmpc.exe /S delete %~nx2.clu /S password %3 _content2.tmp
)
IF "%5" == "" (
	%~dp0hget.exe /C _content.tmp /O * %1
	%~dp0hget.exe /C _content2.tmp /O * %1
) ELSE (
	%~dp0hget.exe /PS %4 /PP %5 /C _content.tmp /O * %1
	%~dp0hget.exe /PS %4 /PP %5 /C _content2.tmp /O * %1
)
DEL _content.tmp
DEL _content2.tmp
CALL %~dp0hput.bat %*
:END
