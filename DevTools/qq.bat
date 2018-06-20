rem usage: qq [-f]
rem if not "%1" == "-f" if /i not "%CD:~0,7%" == "C:\Dev\" goto end -- Žb’è”pŽ~

runsub clean
runsub _clean
moteclean .
solclean .
cx clean **

:end
