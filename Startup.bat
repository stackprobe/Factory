TITLE Factory

CALL "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

ECHO ON

PATH=%PATH%;C:\Factory\Build
PATH=%PATH%;C:\Factory\DevTools
PATH=%PATH%;C:\Factory\Tools

rem ds.exe /s tmp

IF NOT "%1" == "" (
	REM DUMMY
	%*
)
