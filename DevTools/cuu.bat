@set @dirs=C:\Factory C:\Dev C:\pleiades\workspace\Spica01\src C:\pleiades\workspace\Spica02\src C:\pleiades\workspace\Test01\src C:\pleiades\workspace\Test02\src C:\pleiades\workspace\Test03\src-03 C:\pleiades\workspace\Test04\src-04
fndbrknuuid.exe %@dirs%
IF ERRORLEVEL 1 PAUSE
chkuuidcoll.exe %@dirs%
@set @dirs=
