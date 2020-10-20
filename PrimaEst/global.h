/**
 * @file global.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#define _CRT_SECURE_NO_WARNINGS

#define BENCHMARKS

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <assert.h>

 /** ThridParty */
#include "ThirdParty/cJSON/cJSON_Utils.h"
#include "ThirdParty/murmur3/murmur3.h"

/** TOOLS */
#include "Tools/List.h"
#include "Tools/MessageQueue.h"
#include "Tools/Fnv1a.h"
#include "Tools/Crc32.h"
#include "Log/Log.h"
#include "Tools/Bitset.h"

/** UTILS */
#include "Utils/Random.h"

/** STORAGE */
#include "Storage/StorageConfiguration.h"
#include "Storage/Skiplist/SkipList.h"
#include "Storage/Skiplist/LockFreeSkipList.h"
#include "Storage/Skiplist/SkipListDefinitions.h"
#include "Storage/CommitLog/CommitLog.h"

#ifdef TESTS

#include "Test/LockFreeSkipListTest.h"
#include "Test/SkipListTest.h"
#include "Test/LogTest.h"
#include "Test/BitsetTest.h"

#endif

#ifdef BENCHMARKS

#include "Benchmarks/SkipList/SkipListSelection.h"

#endif

#endif // !GLOBAL_H
