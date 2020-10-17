/**
 * @file Selection.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */


/**
 * Выборка основных операций SkipList
 * Insert, Search
 */
VOID
SkipListSelection();


/**
 * Выборка основных операций LockFreeSkipList
 * Insert, Search
 */
VOID
LockFreeSkipListSelection();

/**
 * Выборка Set у LockFree и обычного SkipList
 */
VOID
MultiThreadSetSkipListSelection();


/**
 * Выборка Find у LockFree и обычного SkipList
 */
VOID
MultiThreadFindSkipListSelection();
