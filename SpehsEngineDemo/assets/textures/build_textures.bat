
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET TARGET_LOC=%CD%\..\..\bin\data\textures

REM Clear old textures
IF EXIST %TARGET_LOC% RMDIR /S /Q %TARGET_LOC%
MKDIR %TARGET_LOC%


XCOPY *.png %TARGET_LOC%\ /y


PAUSE
EXIT \B 0
