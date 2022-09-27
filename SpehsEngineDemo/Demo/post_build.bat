ECHO Demo post build script start
ECHO Copy DLLs to TargetDir

DEL %TargetDir%*.dll
COPY %SPEHSENGINE_ROOT%Dependencies\dll\x64\SDL2.dll %TargetDir%SDL2.dll /y

ECHO Demo post build script end