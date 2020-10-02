#pragma once

#define TESTS

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/** TOOLS */
#include "Tools/List.h"

/** STORAGE */
#include "Storage/Skiplist/SkipList.h"

/** UTILS */
#include "Utils/Random.h"


#ifdef TESTS

#include "Test/SkipList/SkipListTest.h"

#endif

#endif // !GLOBAL_H
