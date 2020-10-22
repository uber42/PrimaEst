/**
 * @file SkipListTest.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef LOCK_FREE_SKIP_LIST_TEST_H
#define LOCK_FREE_SKIP_LIST_TEST_H

VOID
LockFreeSkipListAddTest();

VOID
LockFreeSkipListFindTest();

VOID
LockFreeSkipListMultiThreadAddTest();

VOID
LockFreeSkipListMultiThreadFind();

VOID
LockFreeSkipListMultiThreadAddAndFind();

VOID
LockFreeSkipListResetTest();

#endif // !SKIP_LIST_TEST_H
