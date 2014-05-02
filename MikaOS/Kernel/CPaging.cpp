#include "CPaging.h"

CPaging GPaging;

CPaging::CPaging()
{
	PageTable = (unsigned long long*)0x1000;
}

CPaging::~CPaging()
{
}

void* CPaging::AllocNonCacheablePage()
{
	static unsigned short count = 0;
	++count;
	if (0x50000 + count * 0x1000 > 0x200000)
	{
		return (void*)0;
	}
	return (void*)(0x50000LL + count * 0x1000LL);
}