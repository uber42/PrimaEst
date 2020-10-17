/**
 * @file global.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <intrin.h>

 /** ThridParty */
#include "ThirdParty/cJSON/cJSON_Utils.h"
#include "ThirdParty/murmur3/murmur3.h"

/** TOOLS */
#include "Tools/List.h"
#include "Tools/MessageQueue.h"
#include "Tools/Fnv1a.h"
#include "Tools/Crc32.h"
#include "Log/Log.h"

/** STORAGE */
#include "Storage/StorageConfiguration.h"
#include "Storage/Skiplist/SkipList.h"
#include "Storage/Skiplist/LockFreeSkipList.h"
#include "Storage/Skiplist/SkipListDefinitions.h"
#include "Storage/CommitLog/CommitLog.h"

/** UTILS */
#include "Utils/Random.h"

#ifdef TESTS

#include "Test/LockFreeSkipListTest.h"
#include "Test/SkipListTest.h"
#include "Test/LogTest.h"

#endif

#ifdef BENCHMARKS

#include "Benchmarks/SkipList/SkipListSelection.h"

#endif

#endif // !GLOBAL_H
