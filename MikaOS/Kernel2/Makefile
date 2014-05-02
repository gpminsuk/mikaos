ld = ld
ar = x86_64-w64-mingw32-gcc-ar.exe
g++ = g++ -Wall -fno-builtin -nostdlib -ffreestanding -nostdinc -fno-rtti -m64
nasm = nasm
OBJCOPY=objcopy

ASSEMBLY=
SOURCES=Source.cpp
CSOURCES=
OBJECTS=$(ASSEMBLY:.s=.o) $(CSOURCES:.c=.o) $(SOURCES:.cpp=.o)

all: KernelLoader
	
KernelLoader: $(OBJECTS)
	$(ld) -T linkerscript.link -o ../out/Kernel2.dll $(addprefix ./obj/, $(OBJECTS)) ../out/Kernel.dll -estart --image-base 0x20000
	
.s.o:
	$(nasm) -f elf64 $< -o ./obj/$@

.c.o:
	$(g++) -c $< -o ./obj/$@

.cpp.o:
	$(g++) -c $< -o ./obj/$@