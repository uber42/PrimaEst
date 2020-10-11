/**
 * @file CommitLog.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */


typedef struct _SCommitLogSegmentIndex
{
	DWORD	dwOffset;
	DWORD	dwHash;
} SCommitLogSegmentIndex, *PSCommitLogSegmentIndex; 

typedef struct _SCommitLogSegment
{
	DWORD	dwKeySize;
	DWORD	dwValueSize;
} SCommitLogSegment, *PSCommitLogSegment;

typedef struct _SCommitLog
{
	DWORD	dwSegmentsCount;
} *SCommitLog, PSCommitLog;