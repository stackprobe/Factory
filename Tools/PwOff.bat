rem シャットダウンします。
C:\Factory\Tools\wait.exe 60
IF ERRORLEVEL 1 GOTO END
shutdown /s /t 30
rem PAUSE
:END
