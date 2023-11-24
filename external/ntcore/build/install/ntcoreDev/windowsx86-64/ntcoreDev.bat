
@echo off
SETLOCAL

CALL "%~dp0lib\ntcoreDev.exe" %*
EXIT /B %ERRORLEVEL%
ENDLOCAL
