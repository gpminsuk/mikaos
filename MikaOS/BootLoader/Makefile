ld = ld
g++ = g++ -Wall -fno-builtin -nostdlib -ffreestanding -nostdinc -fno-rtti -m64 -std=c++11
nasm = nasm
OBJCOPY=objcopy

ASSEMBLY=AsmEntry.s
SOURCES=Entry.cpp CPaging.cpp ISR.cpp GDT.cpp fat16.cpp fdddrv.cpp vbe.cpp Loader.cpp CDisplayDriver.cpp
CSOURCES=liballoc_1_1.c string.c
OBJECTS=$(ASSEMBLY:.s=.o) $(CSOURCES:.c=.o) $(SOURCES:.cpp=.o)

all: KernelLoader
	
KernelLoader: BootLoader.o $(OBJECTS)
	$(ld) -T linkerscript.link -o ./obj/BootLoader.dll ./obj/BootLoader.o $(addprefix ./obj/, $(OBJECTS))
	$(OBJCOPY) -O binary ./obj/BootLoader.dll ../out/BOOTLDR2.bin

BootLoader.o:
	$(nasm) BootLoader.s -f win64 -o ./obj/BootLoader.o	

.s.o:
	$(nasm) -f elf64 $< -o ./obj/$@

.c.o:
	$(g++) -c $< -o ./obj/$@

.cpp.o:
	$(g++) -c $< -o ./obj/$@