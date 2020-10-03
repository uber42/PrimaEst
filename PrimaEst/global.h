/**
 * @file global.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#define TESTS

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/** ThridParty */
#include "ThirdParty/cJSON/cJSON_Utils.h"

/** TOOLS */
#include "Tools/List.h"
#include "Tools/MessageQueue.h"
#include "Tools/Fnv1a.h"
#include "Tools/Log.h"

/** STORAGE */
#include "Storage/Skiplist/SkipList.h"

/** UTILS */
#include "Utils/Random.h"

#ifdef TESTS

#include "Test/SkipListTest.h"
#include "Test/LogTest.h"

#endif

#endif // !GLOBAL_H
