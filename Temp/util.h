/*
 * util.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mikea
 */

#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <limits.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"

#ifndef RELEASE_BUILD
#define ENABLE_HEAP_DEBUGGING
#endif

extern int32_t g_last_heap_remaining;

// This had better match _Min_Heap_Size in the xxx_FLASH.ld file.
#define ALLOCATED_HEAP_SIZE		0x1400

#ifdef RELEASE_BUILD
#define ENABLE_INTERNAL_WATCHDOG
#endif

#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))

inline void pmdDelay(uint32_t ms){HAL_Delay(ms);}

int noprintf(const char *format, ...);

#ifdef ENABLE_HEAP_DEBUGGING

void *dbg_malloc(size_t size, unsigned int line, const char *file);
void dbg_free(void *ptr, unsigned int line, const char *file);
#define pmdMalloc(size) dbg_malloc(size, __LINE__, 0x0L)
#define pmdFree(ptr) dbg_free(ptr, __LINE__, 0x0L)
extern int app_malloc_times;
#else
void *pmdMalloc(size_t size);
void *pmdCalloc(size_t nitems, size_t size);
void pmdFree(void *ptr);
extern int app_malloc_times;
#endif

inline size_t pmdGetFreeHeapSize(void){return ALLOCATED_HEAP_SIZE - mallinfo().uordblks;}
//inline size_t pmdGetTotalHeapSize(void){return mallinfo().fordblks + mallinfo().uordblks;}
inline size_t pmdGetHeapArenaSize(void){return mallinfo().arena;}
inline size_t pmdGetHeapRemaining(void){return ALLOCATED_HEAP_SIZE - mallinfo().uordblks;}
inline void pmdDoHeapCheck(uint16_t n)
{
	int32_t heap_remaining = pmdGetHeapRemaining();
	printf("Heap Remaining (%d):  %ld (%+ld) \r\n", n, heap_remaining, heap_remaining - g_last_heap_remaining);
	g_last_heap_remaining = heap_remaining;
}

uint8_t Get_CRC8(uint8_t *data, uint16_t size);
uint16_t Get_CRC16_CCITT(uint16_t *data, uint16_t size_hw);
uint16_t Continue_CRC16_CCITT(uint16_t *data, uint16_t size_hw);

int __io_putchar(int ch);
#define FCPutc(c) __io_putchar(c)

void pmdCheckHeapValid(void);
void PetWatchdog(void);

static inline uint32_t pmdGetMsTicks(void)
{
	extern volatile uint32_t uwTick;
	return uwTick;
}

// Note:  TimedOut will work correctly even when ticks roll over (every 49.7 days)
static inline bool TimedOut(uint32_t start, uint32_t period){return (pmdGetMsTicks() - start) > period;}


// TimesUp works because when the left side becomes negative (when time is up) the uint32_t
// value jumps from zero to ULONG_MAX and continues counting down.
static inline bool TimesUp(uint32_t end)
{
  return (end - pmdGetMsTicks()) > (ULONG_MAX >> 1);
}

#define GetMsTicks	pmdGetMsTicks

void CritSect_Push();
void CritSect_Pop();

void LogResetReason(uint32_t rcc_csr);

#define	MODULE_PRINT_DEBUG	0
#if MODULE_PRINT_DEBUG
#include <stdio.h>
#define aci_print(x...)  	printf(x);
#else
#define aci_print(x...)
#endif
extern void pmg_print(const char *fmt, ...)   _ATTRIBUTE ((__format__ (__printf__, 1, 2)));
#endif /* UTIL_H_ */
