#pragma once

void KernelStart(void* arg);
extern "C" void BiosInterrupt(char* VbeGetControllerInfo, unsigned short arg = 0);