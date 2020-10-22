/**
 * @file StorageConfiguration.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef STORAGE_CONFIGURATION
#define STORAGE_CONFIGURATION

/**
 * Конфигурация лога фиксации
 */
typedef struct _SCommitLogConfiguration
{
	DWORD		dwDumpSize;
	DWORD		dwSegmentSize;
	DWORD		dwFlushInterval;
} SCommitLogConfiguration, *PSCommitLogConfiguration;

/**
 * Конфигурация хранилища
 */
typedef struct _SStorageConfiguration
{
	SCommitLogConfiguration		sCommitConfiguration;
} SStorageConfiguration, *PSStorageConfiguration;


#endif