IF NOT EXIST ..\build MKDIR ..\build
MKDIR ..\build\web

call ..\..\src\move-assets.bat >NUL

:: version number replaced here ...
SET emcc=c:\emsdk\emscripten\1.37.22\emcc -v
SET cflags=-O1 -Wall -std=c99 -D_DEFAULT_SOURCE -fgnu89-inline -Wno-missing-braces -s USE_GLFW=3 -s ASSERTIONS=1 --profiling -s TOTAL_MEMORY=16777216 --preload-file resources
SET include=-I.\include

emcc libs\html5\libraylib.bc --shell-file "libs\html5\web_shell\shell.html" -o ..\build\web\index.html .\main.c %cflags% %include% -DPLATFORM_WEB
