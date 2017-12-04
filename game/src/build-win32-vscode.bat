call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"

IF NOT EXIST game\build MKDIR game\build
PUSHD game\build
IF NOT EXIST .\win32 MKDIR win32
PUSHD win32

cl /MD /Z7 /FC /nologo ..\..\src\main.c /I ..\..\src\include /link -incremental:no ..\..\src\libs\win32\msvc\raylib.lib ..\..\src\libs\win32\msvc\glfw3.lib ..\..\src\libs\win32\msvc\OpenAL32.lib gdi32.lib user32.lib shell32.lib

PUSHD ..\..\src
