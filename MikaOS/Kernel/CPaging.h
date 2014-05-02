#pragma once

typedef long long unsigned int	size_t;

class CPaging
{
public:
	CPaging();
	~CPaging();
	
	void* AllocNonCacheablePage();
	unsigned long long* PageTable;
};

extern CPaging GPaging;