@echo off

if not exist build\ (
	mkdir build
)

if not exist external\glfw\ (
	git submodule update --init
	cmake -S external\glfw -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build
)

pushd build
cl /LINK /SUBSYSTEM:WINDOWS /F 16000000 /ZI /MD /Ox /std:c17 ..\src\main.c /I ..\external\glfw\include src\Debug\glfw3.lib winmm.lib gdi32.lib opengl32.lib kernel32.lib user32.lib shell32.lib /D _USE_MATH_DEFINES
popd
