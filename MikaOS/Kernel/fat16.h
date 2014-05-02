#pragma once

#include "Global.h"

#define BYTES_PER_SECTOR				512

struct SFile
{
	bool			found;

	char			*pt_filename;
	unsigned int			attribute;
	unsigned int			filesize;

	unsigned short			cur_cluster;
	unsigned int			bytesread;
	unsigned char			buffer[BYTES_PER_SECTOR];
};

typedef struct _FILE_INFO {
	char filename[8 + 3 + 1 + 1]; /* filename+ext+dot+0, max 12 */
	char attribute;
	unsigned short 			time;
	unsigned short 			date;
	unsigned int 			filesize;
	unsigned short 			start_cluster;
} FILE_INFO, *PFILE_INFO;

void* FsOpenFile(char *filename, unsigned int attribute);
unsigned int FsReadFile(void* handle, char* data, unsigned int numberOfBytesToRead);
void FsCloseFile(void* handle);
void FsGetFileList(bool(*callback)(FILE_INFO*, void*), void* context);
void FsInitializeModule();