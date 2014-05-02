#include "task.h"

struct STaskRingNode
{
	STask* Task;
	STaskRingNode* Next;
};

STask* GCurrentTask;
STaskRingNode* GProcessRing;

void AddTask(STask* addedTask)
{
	STaskRingNode* newNode = new STaskRingNode();
	if (!GProcessRing)
	{
		GProcessRing = newNode;
	}
	newNode->Task = addedTask;
	newNode->Next = GProcessRing->Next;
	GProcessRing->Next = newNode;

	GCurrentTask = GProcessRing->Task;
}

void NextTaskRing()
{
	GProcessRing = GProcessRing->Next;
	GCurrentTask = GProcessRing->Task;
}

void TaskStart()
{
	register unsigned char* stackBase __asm("%rbp");
	(*((void(**)(void*))(stackBase + 16)))(*((void**)(stackBase + 8)));
}

STask* CreateTask(void(*start)(void*), void* argument, unsigned long long flags)
{
	STask* task = new STask();
	task->CodeSegment = 0x08;
	task->StackSegment = 0x10;
	task->Flags = flags;
	AddTask(task);
	task->rsp = new char[2096];
	for (int i = 0; i < 2096; ++i){
		task->rsp[i] = 0;
	}
	task->rsp += 2096;
	task->rsp -= 8;
	(*(void(**)(void*))task->rsp) = start;
	task->rsp -= 8;
	(*(void**)task->rsp) = argument;
	AsmInitializeProcess(TaskStart, task);
	return task;
}