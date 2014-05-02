#include "fat16.h"
#include "fdddrv.h"
#include "string.h"
#include "CDisplayDriver.h"

void FsInitializeModule();

#define NUMBER_OF_DIR_ENTRIES			0xe0
#define CLUSTER_OFFSET					(0x21-2)

typedef struct _BPB
{
	char			BS_jmpBoot[3];
	char			BS_OEMName[8];
	unsigned short			BPB_BytesPerSec;
	char			BPB_SecPerClus;
	unsigned short			BPB_RsvdSecCnt;
	char			BPB_NumFATs;
	unsigned short			BPB_RootEntCnt;
	unsigned short			BPB_TotSec16;
	char			BPB_Media;
	unsigned short			BPB_FATSz16;
	unsigned short			BPB_SecPerTrk;
	unsigned short			BPB_NumHeads;
	unsigned int			BPB_HiddSec;
	unsigned int			BPB_TotSec32;
	char			BS_DrvNum;
	char			BS_Reserved1;
	char			BS_BootSig;
	unsigned int			BS_VolID;
	char			BS_VolLab[11];
	char			BS_FilSysType[8];
} BPB, *PBPB;

typedef struct _DIRECTORY_ENTRY {
	char			filename[8];
	char			extension[3];
	char			attribute;
	char			reserved[10];
	unsigned short			time;
	unsigned short			date;
	unsigned short			cluster;
	unsigned int			filesize;
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

void FspLoadFAT();
void FspLoadDirEntry();
static unsigned short FspGetNextCluster(unsigned short curCluster);
void FsCloseFile(void* handle);

#define FAT_SIZE		(BYTES_PER_SECTOR*9)
unsigned char* m_FAT;
DIRECTORY_ENTRY* m_DirEntry;

void FsInitializeModule()
{
	FspLoadFAT();
	FspLoadDirEntry();
}

void FsGetFileList(bool(*callback)(FILE_INFO*, void*), void* context)
{
	FILE_INFO FileInfo;
	int i, j, k;

	for (i = 0; i<NUMBER_OF_DIR_ENTRIES; i++)
	{
		if (m_DirEntry[i].filename[0] == 0xe5 || m_DirEntry[i].filename[0] == 0)
			continue;

		/* filename */
		j = 0;
		while (m_DirEntry[i].filename[j] != ' ' && j<8) {
			FileInfo.filename[j] = m_DirEntry[i].filename[j];
			j++;
		}
		FileInfo.filename[j++] = '.';

		/* extension */
		k = j;
		j = 0;
		while (m_DirEntry[i].extension[j] != ' ' && j<3) {
			FileInfo.filename[j + k] = m_DirEntry[i].extension[j];
			j++;
		}
		FileInfo.filename[j + k] = '\0';

		/* other info */
		FileInfo.attribute = m_DirEntry[i].attribute;
		FileInfo.time = m_DirEntry[i].time;
		FileInfo.date = m_DirEntry[i].date;
		FileInfo.filesize = m_DirEntry[i].filesize;
		FileInfo.start_cluster = m_DirEntry[i].cluster;
		
		if (!callback(&FileInfo, context))
		{
			break;
		}
	}
}

bool Fsp_of_callback(FILE_INFO* fileInfo, void* context)
{
	SFile* pContext = (SFile*)context;

	if (!strcmp(fileInfo->filename, pContext->pt_filename)) {
		pContext->cur_cluster = fileInfo->start_cluster;
		pContext->filesize = fileInfo->filesize;
		pContext->found = 1;
		return 0;
	}
	return 1;
}

void* FsOpenFile(char *filename, unsigned int attribute)
{
	if (filename == 0 || strlen(filename) == 0)
	{
		return 0;
	}
	strupr(filename);

	SFile* pt_block = new SFile;
	if (pt_block == 0)
	{
		return 0;
	}

	pt_block->found = 0;
	pt_block->pt_filename = filename;
	pt_block->attribute = attribute;
	pt_block->bytesread = 0;

	FsGetFileList(Fsp_of_callback, pt_block);
	if (!pt_block->found)
	{
		CDisplayDriver::PrintString("File not found");
		FsCloseFile(pt_block);
		return 0;
	}
	
	FddpReadWriteSector(FDD_JOB_READ, pt_block->cur_cluster + CLUSTER_OFFSET, 1, pt_block->buffer, BYTES_PER_SECTOR);

	return pt_block;
}

void FsCloseFile(void* handle)
{
	delete (SFile*)handle;
}

unsigned int FsReadFile(void* handle, char* data, unsigned int numberOfBytesToRead)
{
	SFile* pt_block = (SFile*)handle;
	unsigned int bytes_read, bytes_to_read, cur_read_bytes;
	unsigned int pos_of_userdata, pos_of_buffer;

	/* check parameters */
	if (handle == 0 || data == 0 || numberOfBytesToRead == 0) return 0;

	/* check filesize */
	if (pt_block->filesize <= pt_block->bytesread) {
		return 0;
	}
	else if (pt_block->bytesread + numberOfBytesToRead < pt_block->filesize) {
		bytes_to_read = numberOfBytesToRead;
	}
	else { /* pt_block->bytesread+numberOfBytesToRead >= pt_block->filesize */
		bytes_to_read = pt_block->filesize - pt_block->bytesread;
	}

	/* read */
	pos_of_userdata = 0;
	bytes_read = 0;
	while (bytes_to_read != 0) {
		pos_of_buffer = pt_block->bytesread % BYTES_PER_SECTOR;
		cur_read_bytes = BYTES_PER_SECTOR - pos_of_buffer;
		if (cur_read_bytes > bytes_to_read)
			cur_read_bytes = bytes_to_read;

		memcpy((data + pos_of_userdata), (pt_block->buffer + pos_of_buffer), cur_read_bytes);

		bytes_read += cur_read_bytes;
		bytes_to_read -= cur_read_bytes;
		pt_block->bytesread += cur_read_bytes;
		pos_of_userdata += cur_read_bytes;

		if (pt_block->bytesread % BYTES_PER_SECTOR)
			continue;

		pt_block->cur_cluster = FspGetNextCluster(pt_block->cur_cluster);
		if (pt_block->cur_cluster < 0x2 || pt_block->cur_cluster > 0xfef)
			break;

		FddpReadWriteSector(FDD_JOB_READ, pt_block->cur_cluster + CLUSTER_OFFSET, 1, pt_block->buffer, BYTES_PER_SECTOR);
	}

	return bytes_read;
}

void FspLoadFAT()
{
	m_FAT = new unsigned char[FAT_SIZE];
	for (int i = 0; i < FAT_SIZE; ++i)
	{
		m_FAT[i] = 0;
	}
	FddpReadWriteSector(FDD_JOB_READ, 0x1, 9, (unsigned char*)m_FAT, FAT_SIZE);
}

void FspLoadDirEntry()
{
	m_DirEntry = new DIRECTORY_ENTRY[NUMBER_OF_DIR_ENTRIES];
	for (unsigned int i = 0; i < NUMBER_OF_DIR_ENTRIES*sizeof(DIRECTORY_ENTRY); ++i)
	{
		((unsigned char*)m_DirEntry)[i] = 0;
	}
	FddpReadWriteSector(FDD_JOB_READ, 0x13, 14, (unsigned char*)m_DirEntry, NUMBER_OF_DIR_ENTRIES*sizeof(DIRECTORY_ENTRY));
}

unsigned short FspGetNextCluster(unsigned short curCluster)
{
	unsigned short cluster, fat_entry;

	if (curCluster % 2) { /* odd */
		fat_entry = ((curCluster >> 1) * 3) + 1;
	}
	else { /* even */
		fat_entry = ((curCluster >> 1) * 3);
	}

	cluster = (unsigned short)(m_FAT[fat_entry]);
	cluster |= ((unsigned short)(m_FAT[fat_entry + 1]) << 8);

	if (curCluster % 2)
		cluster >>= 4;
	else
		cluster &= 0x0fff;

	return cluster;
}