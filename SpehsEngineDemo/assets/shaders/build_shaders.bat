
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET BUILD_SHADER=%SPEHSENGINE_ROOT%\scripts\build_shader.bat
SET SHADER_DATA_DIR=%CD%\..\..\bin\data\shaders
SET EMBEDDED_PATH=%SPEHSENGINE_ROOT%\SpehsEngine\Graphics\EmbeddedShaders\source


REM Copy shader includes
XCOPY %EMBEDDED_PATH%\*.sh /y


REM Clear old shaders
IF EXIST %SHADER_DATA_DIR% RMDIR /S /Q %SHADER_DATA_DIR%
MKDIR %SHADER_DATA_DIR%


CALL %BUILD_SHADER% %CD% test %SHADER_DATA_DIR%


PAUSE
EXIT \B 0
