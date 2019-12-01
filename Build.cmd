@ECHO OFF

IF "%1" EQU "" (
    ECHO "Usage:"
    ECHO ".\Build.cmd <Puzzle Directory>"
    EXIT /B 1
)

CLS

REM Activate the Developer Command Prompt for Visual Studio 2019, this
REM enables us to use some Visual Studio tools more easily, for example: CL.
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

PUSHD "%1"

IF NOT EXIST ".bin" (
    MKDIR ".bin"
)

PUSHD ".bin\"

CL "..\Solution.cpp" /DDEBUG /Od /Zi

POPD

IF %ERRORLEVEL% EQU 0 (
    CLS
    CALL ".bin\Solution.exe"
)

POPD
