@echo off
echo Building xopenplayer...
gcc -Wall -Wextra -Iinclude src\main.c src\player.c -o xopenplayer -lX11
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)
echo Build succeeded.
