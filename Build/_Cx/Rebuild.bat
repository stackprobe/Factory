START /B /WAIT /DC:\Factory\Common CL /W2 /WX /Oxt /J /GF /c *.c
DEL C:\Factory\Common\memAllocTest.obj
CL /W2 /WX /Oxt /J /GF _Cx.c C:\Factory\Common\*.obj
DEL _Cx.obj
COPY /B _Cx.exe ..\Cx.exe
