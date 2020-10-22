#include "../../global.h"

/**
 * Размеры выборок
 */
static int nSelectionCount[] =
{
	100,
	1000,
	10000,
	100000,
	200000,
	300000,
	400000,
	500000,
	600000,
	700000,
	800000,
	900000,
	1000000
};

VOID
BloomFilterProbablySelection()
{
	//HANDLE hOutFile = CreateFile(
	//	".\\Benchmarks\\BloomFilter\\BloomFilterSelection.bin",
	//	GENERIC_WRITE,
	//	0, NULL,
	//	CREATE_ALWAYS,
	//	0, NULL
	//);
	//if (hOutFile == INVALID_HANDLE_VALUE)
	//{
	//	return;
	//}

	//LONG* plSelection = InternalSelection();

	//DWORD dwBytesWrite = 0;
	//WriteFile(
	//	hOutFile,
	//	plSelection,
	//	sizeof(LONG) * ARRAYSIZE(nSelectionCount),
	//	&dwBytesWrite, NULL);

	//CloseHandle(hOutFile);

	//free(plSelection);
}