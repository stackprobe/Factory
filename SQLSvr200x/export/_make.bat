rem _make.bat SERVER USER PASSWORD
IF "%3" == "" GOTO END

> db.rec.tmp sqlcmd -S %1 -U %2 -P %3 -k2 -Q "select name from [sys].[databases]"

C:\Factory\SQLSvr200x\export\MakeDbList.exe db.rec.tmp db.tmp

C:\Factory\Tools\RDMD.exe /RM out

FOR /F %%D IN (db.tmp) DO (

	> tbl.rec.tmp sqlcmd -S %1 -U %2 -P %3 -k2 -Q "select schema_id, name from [%%D].[sys].[tables]"
	> sch.rec.tmp sqlcmd -S %1 -U %2 -P %3 -k2 -Q "select schema_id, name from [%%D].[sys].[schemas]"

	C:\Factory\SQLSvr200x\export\MakeTableList.exe tbl.rec.tmp sch.rec.tmp %%D tbl.tmp

	FOR /F %%T IN (tbl.tmp) DO (

		> hdr.out.tmp sqlcmd -S %1 -U %2 -P %3 -k2 -Q "select * from %%T where 1 = 2"
		C:\Factory\SQLSvr200x\RecReader.exe hdr.out.tmp hdr.out.tmp

		bcp %%T out tbl.out.tmp -S %1 -U %2 -P %3 -c -t 0x01 -r 0x02
		C:\Factory\SQLSvr200x\BcpReader.exe 01 02 tbl.out.tmp tbl.out.tmp

		COPY /B hdr.out.tmp + tbl.out.tmp out\%%T.csv
	)
)

:END
