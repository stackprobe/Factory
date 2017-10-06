@set @dirs=C:\Factory C:\Dev C:\pleiades\workspace\Test02\src C:\pleiades\workspace\Test03\src-03
fndbrknuuid.exe %@dirs%
IF ERRORLEVEL 1 PAUSE
chkuuidcoll.exe %@dirs%
@set @dirs=
