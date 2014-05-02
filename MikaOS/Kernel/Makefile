ld = ld
g++ = g++ -Wall -fno-builtin -nostdlib -ffreestanding -nostdinc -fno-rtti -m64 -std=c++11
nasm = nasm
OBJCOPY=objcopy

ASSEMBLY=AsmKernel.s Asm16.s
SOURCES=main.cpp task.cpp CPaging.cpp CDisplayDriver.cpp ISR.cpp GDT.cpp Loader.cpp fat16.cpp fdddrv.cpp
CSOURCES=liballoc_1_1.c string.c
OBJECTS=$(ASSEMBLY:.s=.o) $(CSOURCES:.c=.o) $(SOURCES:.cpp=.o)

all: KernelLoader
	
KernelLoader: $(OBJECTS)
	$(g++) -shared -o ../out/Kernel.dll $(addprefix ./obj/, $(OBJECTS)) -estart -Wl,--image-base,0x20000
	
.s.o:
	$(nasm) -f elf64 $< -o ./obj/$@

.c.o:
	$(g++) -c $< -o ./obj/$@

.cpp.o:
	$(g++) -c $< -o ./obj/$@