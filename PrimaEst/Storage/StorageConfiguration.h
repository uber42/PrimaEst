/**
 * @file StorageConfiguration.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

/**
 * ������������ ���� ��������
 */
typedef struct _SCommitLogConfiguration
{
	DWORD		dwDumpSize;
	DWORD		dwSegmentSize;
	DWORD		dwFlushInterval;
} SCommitLogConfiguration, *PSCommitLogConfiguration;

/**
 * ������������ ���������
 */
typedef struct _SStorageConfiguration
{
	SCommitLogConfiguration		sCommitConfiguration;
} SStorageConfiguration, *PSStorageConfiguration;