TITLE Factory

IF EXIST "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" (
	CALL "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
) ELSE (
	IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" (
		CALL "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
	) ELSE (
		rem orz
	)
)
ECHO ON

PATH=%PATH%;C:\Factory\Build
PATH=%PATH%;C:\Factory\DevTools
PATH=%PATH%;C:\Factory\Tools

rem ds.exe /s tmp

IF NOT "%1" == "" (
	REM DUMMY
	%*
)
