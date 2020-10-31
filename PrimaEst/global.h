/**
 * @file global.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#define _CRT_SECURE_NO_WARNINGS

 /** WSA */
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

/** Windows */
#include <Windows.h>

/** STD */
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <assert.h>
#include <math.h>

/** Log */
#include "Log/Log.h"

 /** ThridParty */
#include "ThirdParty/cJSON/cJSON_Utils.h"
#include "ThirdParty/murmur3/murmur3.h"

/** TOOLS */
#include "Tools/List.h"
#include "Tools/MessageQueue.h"
#include "Tools/Fnv1a.h"
#include "Tools/Crc32.h"
#include "Tools/Bitset.h"
#include "Tools/SocketWrapper.h"
#include "Tools/Random.h"

/** STORAGE */
#include "Storage/StorageConfiguration.h"
#include "Storage/Skiplist/SkipList.h"
#include "Storage/Skiplist/LockFreeSkipList.h"
#include "Storage/Skiplist/SkipListDefinitions.h"
#include "Storage/CommitLog/CommitLog.h"
#include "Storage/BloomFilter/BloomFilter.h"

/** CONSENSUS */
#include "Consensus/RPC/DefinitionRPC.h"
#include "Consensus/RPC/ServerRPC.h"
#include "Consensus/RPC/ClientRPC.h"


#ifdef TESTS

#include "Test/LockFreeSkipListTest.h"
#include "Test/SkipListTest.h"
#include "Test/LogTest.h"
#include "Test/BitsetTest.h"
#include "Test/BloomFilterTest.h"
#include "Test/RpcTest.h"

#endif

#ifdef BENCHMARKS

#include "Benchmarks/SkipList/SkipListSelection.h"

#endif

#endif // !GLOBAL_H
