
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET TARGET_LOC=%CD%\..\..\bin\data\fonts

REM Clear old fonts
IF EXIST %TARGET_LOC% RMDIR /S /Q %TARGET_LOC%
MKDIR %TARGET_LOC%


XCOPY *.ttf %TARGET_LOC%\ /y


PAUSE
EXIT \B 0