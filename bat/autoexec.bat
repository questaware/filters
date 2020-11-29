C:\PC-CIL~1\PCSCAN.EXE C:\ C:\WINDOWS\COMMAND\ /NS /WIN95
rem - By Windows Setup - C:\WINDOWS\COMMAND\MSCDEX /V /D:ECSCD003 /M:10
path C:\WINDOWS;C:\WINDOWS\COMMAND;c:\bin;c:\unidos
set tz=GMT1
PATH %PATH%;C:\perl\bin
@echo off
PROMPT $p$g
set TEMP=c:\tmp
set HOME=c:\

lh c:\windows\command\doskey.com /INSERT

mode con codepage prepare=((437) C:\WINDOWS\COMMAND\ega.cpi)
mode con codepage select=437
lh keyb uk,,C:\WINDOWS\COMMAND\keyboard.sys
