#pragma once

#include "Global.h"

class CPaging
{
public:
	CPaging();
	~CPaging();
	
	void* AllocNonCacheablePage();
	unsigned long long* PageTable;
};

extern CPaging GPaging;