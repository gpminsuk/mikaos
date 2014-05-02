cd out
..\mingw64\bin\imagefs c f.img 2880
..\mingw64\bin\imagefs b f.img BootSector.bin
..\mingw64\bin\imagefs a f.img BOOTLDR2.bin
..\mingw64\bin\imagefs a f.img Kernel.dll
..\mingw64\bin\imagefs a f.img Kernel2.dll
pause
bochs.bxrc
pause