#pragma once

#define STASK_FLAG_V86MODE		0x1

struct STask
{
	char* rsp;
	unsigned long long CodeSegment;
	unsigned long long StackSegment;
	unsigned long long Flags;
};

extern "C" STask* GCurrentTask;
extern "C" void NextTaskRing();

extern "C" void AsmInitializeProcess(void(*start)(), struct STask* task);
extern STask* CreateTask(void(*start)(void*), void* argument, unsigned long long flags = 0);