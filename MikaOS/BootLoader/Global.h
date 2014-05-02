typedef long long unsigned int	size_t;
typedef __builtin_va_list		va_list;

extern "C" unsigned char AsmIn(unsigned long long Port);
extern "C" void AsmOut(unsigned long long Port, unsigned char Value);
extern "C" void AsmHalt();
extern "C" void AsmLIDT(class CInterruptHandlerDescriptor* Descriptor);
extern "C" void AsmLGDT(class CGlobalDescriptorTable* Descriptor);
extern "C" void AsmLTR(unsigned int index);
extern "C" void AsmIntSoftwareContextSwitch();
extern "C" void AsmFirstSoftwareContextSwitch(struct STask* rootTask);
extern "C" void AsmInitializeProcess(void(*start)(), struct STask* task);
extern "C" void AsmSoftwareContextSwitchByInterrupt();
extern "C" void AsmSoftwareContextSwitchBySystemCall();
extern "C" void VbeGetControllerInfo();
extern "C" void VbeGetModeInfo();
extern "C" void VbeSetBank();

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_arg(v,l)		__builtin_va_arg(v,l)
#define va_end(v)		__builtin_va_end(v)
#define va_copy(d,s)	__builtin_va_copy(d,s)

void* operator new[](size_t size);
void operator delete[](void* ptr);
void* operator new(size_t size);
void operator delete(void* ptr);