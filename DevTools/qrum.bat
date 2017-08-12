rem usage: qrum [(-fÅb--) [-q]]
if not "%1" == "-f" if /i not "%CD:~0,7%" == "C:\Dev\" goto end

call qq -f
if not "%2" == "-q" (
rum .
) else (
rum /q .
)
rum /rrr .

:end
