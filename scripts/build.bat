@echo off

set "VSCMD_START_DIR=%cd%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set release_flags= -MT -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Oi -WX -W4
set debug_flags= -Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -Z7
set linker_flags= user32.lib gdi32.lib

if not exist ..\build mkdir ..\build

pushd ..\build

cl %release_flags% -Fewin32.exe ..\src\win32_main.c %linker_flags%
cl %debug_flags% -Fewin32d.exe ..\src\win32_main.c %linker_flags%

popd
