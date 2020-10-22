/**
 * @file BitsetTest.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"


VOID
BitsetSetBitTest()
{
	DWORD dwCount = 15;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	BS_TYPE bsMask = 0xA;

	assert(BitsetSetBit(psBitset, 1) == TRUE);
	assert(BitsetSetBit(psBitset, 3) == TRUE);
	assert(BitsetSetBit(psBitset, 17) == FALSE);

	BS_TYPE pbsBuffer = *psBitset->pllSet;

	BS_TYPE bsAnd = bsMask & pbsBuffer;
	assert(bsAnd == bsMask);

	DeleteBitset(psBitset);
	
	dwCount = 90;

	InitializeBitset(&sBitset, dwCount);

	BS_TYPE bsMask_1 = 0xA;
	BS_TYPE bsMask_2 = 0x42;

	assert(BitsetSetBit(psBitset, 1) == TRUE);
	assert(BitsetSetBit(psBitset, 3) == TRUE);

	assert(BitsetSetBit(psBitset, 65) == TRUE);
	assert(BitsetSetBit(psBitset, 70) == TRUE);

	assert(BitsetSetBit(psBitset, 91) == FALSE);

	pbsBuffer = *psBitset->pllBegin;

	assert((bsMask_1 & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	assert((bsMask_2 & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet8MaskTest()
{
	DWORD dwCount = 90;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	BS_TYPE bsMask_1 = 0xA;
	BS_TYPE bsMask_2 = 0x11;

	assert(BitsetSet8Mask(psBitset, (BYTE)bsMask_1, 5) == TRUE);
	assert(BitsetSet8Mask(psBitset, (BYTE)bsMask_2, 70) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	bsMask_1 <<= 5;
	assert((bsMask_1 & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	bsMask_2 <<= 6;
	assert((bsMask_2 & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);


	dwCount = 90;

	InitializeBitset(&sBitset, dwCount);

	BS_TYPE bsMask_3 = 0xFF;

	assert(BitsetSet8Mask(psBitset, (BYTE)bsMask_3, 60) == TRUE);

	pbsBuffer = *psBitset->pllBegin;
	bsMask_3 <<= 60;
	assert((bsMask_3 & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	bsMask_3 = 0xF;
	assert((bsMask_3 & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet16MaskTest()
{
	DWORD dwCount = 90;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BS_TYPE bsMask = 0xFFFF;

	assert(BitsetSet16Mask(psBitset, (WORD)bsMask, 56) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	bsMask <<= 56;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	bsMask = 0xFF;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet64Mask_x86Test()
{
	DWORD dwCount = 250;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;
	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	DWORD pbsMask[2] = { 0xAAAAAAAA, 0xFFFFFFFF };
	BS_TYPE bsMask = 0xFFFFFFFFAAAAAAAA;

	assert(BitsetSet64Mask_x86(psBitset, pbsMask, 32) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	bsMask <<= 32;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	bsMask = 0xFFFFFFFF;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet128Mask_x86Test()
{
	DWORD dwCount = 300;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;
	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	DWORD pbsMask[4] = { 0xAAAAAAAA, 0xFFFFFFFF, 0xBBBBBBBB, 0xCCCCCCCC };
	BS_TYPE bsMask[3] = { 0xAAAAAAAA00000000, 0xBBBBBBBBFFFFFFFF, 0x00000000CCCCCCCC };

	assert(BitsetSet128Mask_x86(psBitset, pbsMask, 32) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	assert((bsMask[0] & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	assert((bsMask[1] & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 2);
	assert((bsMask[2] & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet64Mask_x64Test()
{
	DWORD dwCount = 250;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BS_TYPE bsMask = 0xFFFFFFFFAAAAAAAA;

	assert(BitsetSet64Mask_x64(psBitset, bsMask, 32) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	bsMask <<= 32;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	bsMask = 0xFFFFFFFF;
	assert((bsMask & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetSet128Mask_x64Test()
{
	DWORD dwCount = 300;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BS_TYPE pbsMask[2] = { 0xFFFFFFFFAAAAAAAA, 0xCCCCCCCCBBBBBBBB };
	BS_TYPE bsMask[3] = { 0xAAAAAAAA00000000, 0xBBBBBBBBFFFFFFFF, 0x00000000CCCCCCCC };

	assert(BitsetSet128Mask_x64(psBitset, pbsMask, 32) == TRUE);

	BS_TYPE pbsBuffer = *psBitset->pllBegin;
	assert((bsMask[0] & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 1);
	assert((bsMask[1] & pbsBuffer) == pbsBuffer);

	pbsBuffer = *(psBitset->pllBegin - 2);
	assert((bsMask[2] & pbsBuffer) == pbsBuffer);

	DeleteBitset(psBitset);
}

VOID
BitsetCheckBitTest()
{
	DWORD dwCount = 15;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;
	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	BS_TYPE bsMask = 0xA;

	assert(BitsetSetBit(psBitset, 1) == TRUE);
	assert(BitsetSetBit(psBitset, 3) == TRUE);
	assert(BitsetSetBit(psBitset, 17) == FALSE);

	BS_TYPE pbsBuffer = *psBitset->pllSet;

	BS_TYPE bsAnd = bsMask & pbsBuffer;
	assert(bsAnd == bsMask);

	assert(BitsetCheckBit(psBitset, 1) == TRUE);
	assert(BitsetCheckBit(psBitset, 3) == TRUE);
	assert(BitsetCheckBit(psBitset, 17) == FALSE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck8MaskTest()
{
	DWORD dwCount = 90;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BYTE bsMask_1 = 0xA;
	BYTE bsMask_2 = 0x11;

	assert(BitsetSet8Mask(psBitset, bsMask_1, 5) == TRUE);
	assert(BitsetSet8Mask(psBitset, bsMask_2, 70) == TRUE);

	assert(BitsetCheck8Mask(psBitset, bsMask_1, 5) == TRUE);
	assert(BitsetCheck8Mask(psBitset, bsMask_2, 70) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck64Mask_x86Test()
{
	DWORD dwCount = 250;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	DWORD pbsMask[2] = { 0xAAAAAAAA, 0xFFFFFFFF };

	assert(BitsetSet64Mask_x86(psBitset, pbsMask, 32) == TRUE);

	assert(BitsetCheck64Mask_x86(psBitset, pbsMask, 32) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck128Mask_x86Test()
{
	DWORD dwCount = 300;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	DWORD pbsMask[4] = { 0xAAAAAAAA, 0xFFFFFFFF, 0xBBBBBBBB, 0xCCCCCCCC };
	BS_TYPE bsMask[3] = { 0xAAAAAAAA00000000, 0xBBBBBBBBFFFFFFFF, 0x00000000CCCCCCCC };

	assert(BitsetSet128Mask_x86(psBitset, pbsMask, 32) == TRUE);

	assert(BitsetCheck128Mask_x86(psBitset, pbsMask, 32) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck64Mask_x64Test()
{
	DWORD dwCount = 250;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BS_TYPE bsMask = 0xFFFFFFFFAAAAAAAA;

	assert(BitsetSet64Mask_x64(psBitset, bsMask, 32) == TRUE);

	assert(BitsetCheck64Mask_x64(psBitset, bsMask, 32) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck128Mask_x64Test()
{
	DWORD dwCount = 300;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;

	BS_TYPE pbsMask[2] = { 0xFFFFFFFFAAAAAAAA, 0xCCCCCCCCBBBBBBBB };
	BS_TYPE bsMask[3] = { 0xAAAAAAAA00000000, 0xBBBBBBBBFFFFFFFF, 0x00000000CCCCCCCC };

	assert(BitsetSet128Mask_x64(psBitset, pbsMask, 32) == TRUE);

	assert(BitsetCheck128Mask_x64(psBitset, pbsMask, 32) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck128MaskAligned_x86Test()
{
	DWORD dwCount = 512;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;
	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	DWORD pbsMask[4] = { 0xAAAAAAAA, 0xFFFFFFFF, 0xBBBBBBBB, 0xCCCCCCCC };

	BitsetSet128MaskAligned_x86(psBitset, pbsMask, 0);

	assert(BitsetCheck128MaskAligned_x86(psBitset, pbsMask, 0) == TRUE);

	DeleteBitset(psBitset);
}

VOID
BitsetCheck128MaskAligned_x64Test()
{
	DWORD dwCount = 512;

	SBitset sBitset;
	InitializeBitset(&sBitset, dwCount);
	PSBitset psBitset = &sBitset;
	DWORD dwExceptedBlockCount = dwCount / (sizeof(BS_TYPE) * 8) + ((dwCount % (sizeof(BS_TYPE) * 8)) == 0 ? 0 : 1);
	assert(psBitset->dwBlocksCount == dwExceptedBlockCount);

	BS_TYPE pbsMask[2] = { 0xFFFFFFFFAAAAAAAA, 0xCCCCCCCCBBBBBBBB };

	BitsetSet128MaskAligned_x64(psBitset, pbsMask, 0);

	assert(BitsetCheck128MaskAligned_x64(psBitset, pbsMask, 0) == TRUE);

	DeleteBitset(psBitset);
}