#include "Loader.h"
#include "task.h"
#include "fat16.h"
#include "CDisplayDriver.h"

extern "C" bool Execute(char* filename)
{
	void* fp = FsOpenFile(filename, 0);
	SPEDOSHeader dosHeader;
	FsReadFile(fp, (char*)&dosHeader, sizeof(SPEDOSHeader));
	int dumpsize = dosHeader.HeaderAddress - sizeof(SPEDOSHeader);
	char* dump = new char[dumpsize];
	FsReadFile(fp, dump, dumpsize);
	delete[] dump;
	SPEHeader peHeader;
	FsReadFile(fp, (char*)&peHeader, sizeof(SPEHeader));
	SPEOptionalHeader peoHeader;
	FsReadFile(fp, (char*)&peoHeader, sizeof(SPEOptionalHeader));
	FsCloseFile(fp);
	char* memory = new char[peoHeader.SizeOfImage];
	SPEFile outPEFile;
	LoadPE(filename, memory, &outPEFile);
	__asm("cli;hlt;");
	CDisplayDriver::PrintString("%X", outPEFile.peoHeader->AddressOfEntryPoint);
	//STask* newTask = CreateTask((void(*)(void*))outPEFile.peoHeader->AddressOfEntryPoint, 0);
}

extern "C" bool LoadPE(char* filename, char* address, SPEFile* outPEFile)
{
	void* fp = FsOpenFile(filename, 0);
	outPEFile->dosHeader = (SPEDOSHeader*)address;
	FsReadFile(fp, (char*)outPEFile->dosHeader, sizeof(SPEDOSHeader));
	if (outPEFile->dosHeader->Magic != DOS_HEADER_MAGIC)
	{
		CDisplayDriver::PrintString("%X DOS_HEADER_MAGIC is wrong!", outPEFile->dosHeader->Magic);
		FsCloseFile(fp);
		return false;
	}
	int dumpsize = outPEFile->dosHeader->HeaderAddress - sizeof(SPEDOSHeader);
	char* dump = new char[dumpsize];
	FsReadFile(fp, dump, dumpsize);
	delete[] dump;
	outPEFile->peHeader = (SPEHeader*)(address + outPEFile->dosHeader->HeaderAddress);
	FsReadFile(fp, (char*)outPEFile->peHeader, sizeof(SPEHeader));
	if (outPEFile->peHeader->Magic != PE_HEADER_MAGIC)
	{
		CDisplayDriver::PrintString("PE_HEADER_MAGIC is wrong!");
		FsCloseFile(fp);
		return false;
	}
	outPEFile->peoHeader = (SPEOptionalHeader*)((char*)outPEFile->peHeader + sizeof(SPEHeader));
	FsReadFile(fp, (char*)outPEFile->peoHeader, sizeof(SPEOptionalHeader));
	if (outPEFile->peoHeader->Magic != PE_OPTIONAL_MAGIC_64)
	{
		
		CDisplayDriver::PrintString("PE_OPTIONAL_MAGIC_64 is wrong!");
		FsCloseFile(fp);
		return false;
	}
	outPEFile->dataDirectories = (SPEDataDirectories*)((char*)outPEFile->peoHeader + sizeof(SPEOptionalHeader));
	FsReadFile(fp, (char*)outPEFile->dataDirectories, sizeof(SPEDataDirectories));
	outPEFile->sections = new SPESection*[outPEFile->peHeader->NumberOfSections];
	for (unsigned int i = 0; i < outPEFile->peHeader->NumberOfSections; ++i)
	{
		outPEFile->sections[i] = (SPESection*)((char*)outPEFile->dataDirectories + sizeof(SPEDataDirectories)+(i*sizeof(SPESection)));
		FsReadFile(fp, (char*)outPEFile->sections[i], sizeof(SPESection));
		CDisplayDriver::PrintString("%s", outPEFile->sections[i]->Name);
	}
	for (unsigned int i = 0; i < outPEFile->peHeader->NumberOfSections; ++i)
	{		
		if (outPEFile->sections[i]->SizeOfRawData == 0)
		{
			continue;
		}
		int dumpsize = outPEFile->sections[i]->PointerToRawData - ((SFile*)fp)->bytesread;
		char* dump = new char[dumpsize];
		FsReadFile(fp, dump, dumpsize);
		delete[] dump;
		FsReadFile(fp, (char*)address + outPEFile->sections[i]->VirtualAddress, outPEFile->sections[i]->SizeOfRawData);
	}
	outPEFile->exportDirectoryTable = (SPEExportDirectoryTable*)((char*)address + outPEFile->dataDirectories->ExportTable.VirtualAddress);
	outPEFile->importDirectoryTable = (SPEImportDirectoryTable*)((char*)address + outPEFile->dataDirectories->ImportTable.VirtualAddress);	
	if (*(unsigned long long*)outPEFile->importDirectoryTable)
	{
		SPEImportLookupTable* start = (SPEImportLookupTable*)(address + outPEFile->importDirectoryTable->ImportLookupTableRVA);
		int count = 0;
		while (*(unsigned long long*)start)
		{
			start++;
			count++;
		}
		start = (SPEImportLookupTable*)(address + outPEFile->importDirectoryTable->ImportLookupTableRVA);
		outPEFile->importLookups = new SPEImportLookupTable*[count];
		int i = 0;
		while (*(unsigned long long*)start)
		{
			bool isOrdinal = start->LookupEntry & 0x8000000000000000;
			if (isOrdinal)
			{
			}
			else
			{
				unsigned long long HintNameRVA = start->LookupEntry & ~0x8000000000000000;
				SPEImportHintNameTable* HintNameTable = (SPEImportHintNameTable*)(address + HintNameRVA);
				//CDisplayDriver::PrintString("%s", &HintNameTable->Name);
				count++;
			}
			outPEFile->importLookups[i]->LookupEntry = *(unsigned long long*)start;
			start++;
			i++;
		}

		unsigned long long* startAddr = (unsigned long long*)(address + outPEFile->importDirectoryTable->ImportAddressTableRVA);
		while (*startAddr)
		{
			*startAddr += 0x20000;
			startAddr++;
		}
	}	
	FsCloseFile(fp);
	return true;
}