
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET BUILD_SHADER=%SPEHSENGINE_ROOT%\Dependencies\tools\build_shader.bat
SET SHADER_DATA_DIR=%CD%\..\..\bin\data\shaders


CALL %BUILD_SHADER% %CD% test %SHADER_DATA_DIR%

PAUSE
EXIT \B 0
