#include "GDT.h"

unsigned char CGlobalDescriptorEntry::Entries[NUM_GDT_ENTRIES];
CTaskStateSegment CGlobalDescriptorEntry::TaskStateSegments[NUM_GDT_ENTRY_TSS];