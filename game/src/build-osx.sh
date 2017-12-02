mkdir ../build
mkdir ../build/osx

#:: version number replaced here ...
export cflags="-O1 -Wall -std=c99 -D_DEFAULT_SOURCE -fgnu89-inline -Wno-missing-braces -s -USE_GLFW=3 -m32"
export include="-I./include"
export linker="-L../src/libs/"

#gcc -o ../build/osx/main $cflags ./main.c $include $linker
clang -I ~/src/raylib/src -L /usr/local/lib -L ~/src/raylib/release/libs/osx -lglfw -lraylib -framework GLUT -framework OpenGL -framework Cocoa ./main.c -o ../build/osx/main
