set PATH=%PATH%;..\mingw64\bin
set PATH=%PATH%;..\nasm-2.11.02
nasm ./BootSector.s -f bin -o ../out/BootSector.bin