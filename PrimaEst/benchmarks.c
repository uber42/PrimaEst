﻿/**
 * @file benchmarks.c
 * * @author Pavel Chursin
 * @date Oct 2020
 */

#include "global.h"

#ifdef BENCHMARKS

int main()
{
	//BloomFilterSelection();

	//SkipListSelection();
	//MultiThreadRemoveSkipListSelection();
	//LockFreeSkipListSelection();
	//MultiThreadFindSkipListSelection();
	MultiThreadSetSkipListSelection();
}

#endif