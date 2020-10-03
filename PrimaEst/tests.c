/**
 * @file tests.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "global.h"

int main()
{
	//SkipListAddTest();
	//SkipListSetTest();

	LogEnumFnvTest();
	ParseLogTest_1();
	ParseLogTest_2();

	system("PAUSE");
}