rem usage: qrum [-f]
if not "%1" == "-f" if /i not "%CD:~0,7%" == "C:\Dev\" goto end

call qq -f
rum .
rum /rrr .

:end
