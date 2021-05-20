
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET TEXTUREC=%SPEHSENGINE_ROOT%\Dependencies\tools\bgfx\texturecRelease.exe
SET TOKTX=%SPEHSENGINE_ROOT%\Dependencies\tools\ktx\toktx.exe
SET TARGET_LOC=%CD%\..\..\bin\data\textures

REM Clear old textures
IF EXIST %TARGET_LOC% RMDIR /S /Q %TARGET_LOC%
MKDIR %TARGET_LOC%


XCOPY *.png %TARGET_LOC%\ /y

%TOKTX% --cubemap %TARGET_LOC%\skybox.ktx skybox\right.png skybox\left.png skybox\top.png skybox\bottom.png skybox\front.png skybox\back.png


ECHO.
ECHO Done.
PAUSE
EXIT \B 0
