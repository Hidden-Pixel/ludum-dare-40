## Ludum Dare 40
TODO(nick): game description here

### Development Setup
Brief summary of the list of dependencies and steps needed in order to build the project.

Project uses following libraries:
- [raylib][raylib]
  - [download windows installer][raylib-dl]
  - [wiki][raylib-wiki]
  - [cheatsheet][raylib-cheatsheet]

#### Win32
	- Compiler: MSVC[msvc] (visual studio / c and c++ installation only)
	- Env Script: Shell-win32.bat
		- environment is contained with cmd instance running another compiler could mess with set envs.
	- Build Script: build-win32.bat

#### Linux/Mac
TODO(nick)

#### Web
- SDK: emcscriptensdk
  - [download sdk][emscripten-sdk]
- Compiler: emcc

##### emcc - emscripten setup

### Backup Plan Development Setup
When in doubt, we should probably switch over to the threejs-alt folder that uses the javascript threejs library to get something done!

[emscripten-sdk]:http://kripken.github.io/emscripten-site/
[msvc]: https://www.visualstudio.com/vs/community/
[raylib]: https://github.com/raysan5/raylib
[raylib-cheatsheet]:http://www.raylib.com/cheatsheet/cheatsheet.html
[raylib-dl]:https://raysan5.itch.io/raylib/download/eyJleHBpcmVzIjoxNTEyMTk5MzA4LCJpZCI6ODUzMzF9.LqUq4shrk%2b6p%2faTV7y7CumPj%2b5w%3d 
[raylib-wiki]:https://github.com/raysan5/raylib/wiki 
