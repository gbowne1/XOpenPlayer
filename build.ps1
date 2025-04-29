Write-Output "Compiling with GCC..."
gcc -Wall -Wextra -Iinclude src/main.c src/player.c -o xopenplayer -lX11
Write-Output "Build finished. Run with ./xopenplayer"
