
@echo off
SETLOCAL

CALL "%~dp0lib\ntcoreTest.exe" %*
EXIT /B %ERRORLEVEL%
ENDLOCAL
