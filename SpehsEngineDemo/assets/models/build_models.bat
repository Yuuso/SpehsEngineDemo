
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET TARGET_LOC=%CD%\..\..\bin\data\models

REM Clear old models
IF EXIST %TARGET_LOC% RMDIR /S /Q %TARGET_LOC%
MKDIR %TARGET_LOC%


XCOPY *.fbx %TARGET_LOC%\ /y
XCOPY *.dae %TARGET_LOC%\ /y
XCOPY *.obj %TARGET_LOC%\ /y
XCOPY *.gltf %TARGET_LOC%\ /y
XCOPY *.glb %TARGET_LOC%\ /y


PAUSE
EXIT \B 0
