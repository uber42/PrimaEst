/**
 * @file BloomFilter.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

/**
 * Установка платфомозависимых функций 
 */

#if defined(_M_X64) || defined(_M_AMD64)

#define MurMurHash128 MurmurHash3_x64_128
#define BitsetSet64Mask BitsetSet64Mask_x64
#define BitsetSet128Mask BitsetSet128Mask_x64

#elif defined(_M_IX86)

#define MurMurHash128 MurmurHash3_x86_128
#define BitsetSet64Mask BitsetSet64Mask_x86
#define BitsetSet128Mask BitsetSet128Mask_x86

#else

#endif