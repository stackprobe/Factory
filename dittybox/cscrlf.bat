FOR %%D IN (
	C:\Dev\CSharp
	C:\Dev\Kit
) DO (
	lss %%D \LICENSE* .cs* .sln* .csproj* .user* .resx* .settings* .bat* .txt*
	crlf /crlf /lss
)
