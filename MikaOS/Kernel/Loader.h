#pragma once

#define DOS_HEADER_MAGIC		0x5A4D
#define PE_HEADER_MAGIC			0x4550
#define PE_OPTIONAL_MAGIC_32	0x010B
#define PE_OPTIONAL_MAGIC_64	0x020B

#pragma pack(push, 1)
struct SPEDOSHeader
{
	unsigned short Magic;
	unsigned char None[58];
	unsigned int HeaderAddress;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct SPEHeader
{
	unsigned int Magic;
	unsigned short Machine;
	unsigned short NumberOfSections;
	unsigned int TimeDateStamp;
	unsigned int PointerToSymbolTable;
	unsigned int NumberOfSymbols;
	unsigned short SizeOfOptionalHeader;
	unsigned short Characteristics;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEOptionalHeader
{
	unsigned short Magic;
	unsigned char MajorLinkerVersion;
	unsigned char MinorLinkerVersion;
	unsigned int SizeOfCode;
	unsigned int SizeOfInitializedData;
	unsigned int SizeOfUninitializedData;
	unsigned int AddressOfEntryPoint;
	unsigned int BaseOfCode;
	unsigned long long ImageBase;
	unsigned int SectionAlignment;
	unsigned int FileAlignment;
	unsigned short MajorOperatingSystemVersion;
	unsigned short MinorOperatingSystemVersion;
	unsigned short MajorImageVersion;
	unsigned short MinorImageVersion;
	unsigned short MajorSubsystemVersion;
	unsigned short MinorSubsystemVersion;
	unsigned int Win32VersionValue;
	unsigned int SizeOfImage;
	unsigned int SizeOfHeaders;
	unsigned int CheckSum;
	unsigned short Subsystem;
	unsigned short DllCharacteristics;
	unsigned long long SizeOfStackReserve;
	unsigned long long SizeOfStackCommit;
	unsigned long long SizeOfHeapReserve;
	unsigned long long SizeOfHeapCommit;
	unsigned int LoaderFlags;
	unsigned int NumberOfRvaAndSizes;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEDataDirectory
{
	unsigned int VirtualAddress;
	unsigned int Size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEDataDirectories
{
	SPEDataDirectory ExportTable;
	SPEDataDirectory ImportTable;
	SPEDataDirectory ResourceTable;
	SPEDataDirectory ExceptionTable;
	SPEDataDirectory CertificateTable;
	SPEDataDirectory BaseRelocationTable;
	SPEDataDirectory Debug;
	SPEDataDirectory Architecture;
	SPEDataDirectory GlobalPtr;
	SPEDataDirectory TLSTable;
	SPEDataDirectory LoadConfigTable;
	SPEDataDirectory BoundImport;
	SPEDataDirectory IAT;
	SPEDataDirectory DelayImportDescriptor;
	SPEDataDirectory CLRRuntimeHeader;
	SPEDataDirectory Reserved;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPESection
{
	char Name[8];
	unsigned int VirtualSize;
	unsigned int VirtualAddress;
	unsigned int SizeOfRawData;
	unsigned int PointerToRawData;
	unsigned int PointerToRelocations;
	unsigned int PointerToLineNumbers;
	unsigned short NumberOfRelocations;
	unsigned short NumberOfLineNumbers;
	unsigned int Characteristics;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEExportDirectoryTable
{
	unsigned int ExportFlags;
	unsigned int TimeDateStamp;
	unsigned short MajorVersion;
	unsigned short MinorVersion;
	unsigned int NameRVA;
	unsigned int OrdinalBase;
	unsigned int AddressTableEntries;
	unsigned int NumberOfNamePointers;
	unsigned int ExportAddressTableRVA;
	unsigned int NamePointerRVA;
	unsigned int OrdinalTableRVA;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEExportAddressTable
{
	unsigned int ExportRVA;
	unsigned int ForwarderRVA;	
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEImportDirectoryTable
{
	unsigned int ImportLookupTableRVA;
	unsigned int TimeDateStamp;
	unsigned int ForwarderChain;
	unsigned int NameRVA;
	unsigned int ImportAddressTableRVA;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEImportLookupTable
{
	unsigned long long LookupEntry;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEImportHintNameTable
{
	unsigned short Hint;
	char Name;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEImportAddressTable
{
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SPEFile
{
	SPEDOSHeader* dosHeader;
	SPEHeader* peHeader;
	SPEOptionalHeader* peoHeader;
	SPEDataDirectories* dataDirectories;
	SPESection** sections;
	SPEExportDirectoryTable* exportDirectoryTable;
	SPEImportDirectoryTable* importDirectoryTable;
	SPEImportLookupTable** importLookups;
};
#pragma pack(pop)

extern "C" bool LoadPE(char* filename, char* address, SPEFile* outPEFile);
extern "C" bool Execute(char* filename);