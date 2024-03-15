#include <stdlib.h>
#include <malloc.h>
#include "msl_main.h"
//#include "stm32l0xx_hal.h"
#include "util.h"
#include "core_cm0plus.h"

volatile uint32_t ticks = 0;

extern uint8_t DisplayUpdateThread_Alive;

#ifdef ENABLE_INTERNAL_WATCHDOG
extern IWDG_HandleTypeDef hiwdg;
#endif

int32_t g_last_heap_remaining = ALLOCATED_HEAP_SIZE;

void PetWatchdog(void)
{
	// Make sure the display thread is continuing to execute
#ifdef ENABLE_INTERNAL_WATCHDOG
		// Pet the internal watchdog
		HAL_IWDG_Refresh(&hiwdg);
#endif
}

#ifdef ENABLE_HEAP_DEBUGGING

static int32_t dbg_last_heap_remaining = ALLOCATED_HEAP_SIZE;

typedef struct
{
	uint32_t	Magic;
	uint32_t	Size;
	uint32_t	Payload[0];
}mlc_struct;
#define MAGIC_NUM		0xDEADBEEF
int app_malloc_times=0;
static void *ptr_stack[64];
static volatile char  ptr_rel[64];
static short int  ptr_len[64];
static short int  ptr_line[64];
static void store_ptr(void *ptr);
static void store_ptr(void *ptr)
{
	int i;
	i=app_malloc_times;
	if(i>63)
		return;
	ptr_stack[i]=ptr;
	ptr_rel[i]='M';
	app_malloc_times++;
}

static int check_ptr(void *ptr);
static int check_ptr(void *ptr)
{
	int i,j;
	i=app_malloc_times;
	if(i>63)
		return 1;
	for(j=i-1;j>0;j--)
	{
		if(ptr==ptr_stack[j])
		{
			ptr_rel[j]='F';
			return (i-j);
		}
		continue;
	}
	return 0;
}

static int dec_counter(void);
static int dec_counter()
{
	int i,j;
	i=app_malloc_times;
	if(i>63)
		return 0;
	for(j=i-1;j>0;j--)
	{
		if(ptr_rel[j]=='F')
			app_malloc_times--;
		else
			break;
	}
	return j;
}

void *dbg_malloc(size_t size, unsigned int line, const char *file)
{
	// Store the size with the block being alloc'd
	mlc_struct *mlc = (mlc_struct*)malloc(size + 8);
	mlc->Size = size;
	mlc->Magic = MAGIC_NUM;
	void *mem = (void*)mlc->Payload;
	store_ptr(mem);
	int i;
	i=app_malloc_times;
	ptr_len[i]=size;
	ptr_line[i]=line;
	if(!mlc)
	{
		// No point in going on
		// Need to come back and add something to aid post-mortem analysis
		// Maybe printf will still work...
		printf("\r\nERROR:  malloc failed!\r\n");
#ifndef RELEASE_BUILD
		while(1);
#endif
		NVIC_SystemReset();
	}

//	dbg_last_heap_remaining -= size + 8;
//	if((line!=74)&&(line!=77))
//	printf("<MHEAP>, '%08X', +%d, %ld, %d, %s\r\n", (unsigned int)mem, size + 8, dbg_last_heap_remaining, line, file);

	return mem;
}
void dbg_free(void *ptr, unsigned int line, const char *file)
{
	int ret;
	if((unsigned int)ptr<0x20000000)
		return;
	if((unsigned int)ptr>0x20004f00)
		return;
	CritSect_Push();
	ret=check_ptr(ptr);
	if(ret==0)
	{
//		printf("\r\nERROR in dbg_free, %08X invalid address!\r\n",(unsigned int)ptr);
	}
	dec_counter();
	CritSect_Pop();
	mlc_struct *mlc = (mlc_struct*)(((uint8_t*)ptr) - 8);

	if((uint32_t)mlc & 0x00000003)
	{
//		printf("\r\nERROR in dbg_free:  unaligned access attempt!\r\n");
		return;
	}

	if(mlc->Magic != MAGIC_NUM)
	{
//		printf("\r\nERROR in dbg_free,  Magic number mismatch! %d %d\r\n",mlc->Magic,mlc->Size);
		return;
	}

//	dbg_last_heap_remaining += mlc->Size + 8;

//	if((line!=148)&&(line!=149)&&(line!=565))
//	printf("<FHEAP>, '%08X', -%ld, %ld, %d, %s\r\n", (unsigned int)ptr, mlc->Size + 8, dbg_last_heap_remaining, line, file);

	mlc->Magic=0x55aaaa55;
	CritSect_Push();
	free((void*)mlc);
	CritSect_Pop();
}
#else
int app_malloc_times=0;
void *pmdMalloc(size_t size)
{
	void *mem = malloc(size);

//	printf("malloc 0x%08X, %d:  ", (uint32_t)mem, size);
//	pmdDoHeapCheck(82);

	if(!mem)
	{
		// No point in going on
		// Need to come back and add something to aid post-mortem analysis
		// Maybe printf will still work...
		printf("\r\nERROR:  malloc failed!\r\n");
#ifndef RELEASE_BUILD
		while(1);
#endif
		NVIC_SystemReset();
	}
//	printf("[MRem %d]", pmdGetHeapRemaining());
	app_malloc_times++;
	return mem;
}

void *pmdCalloc(size_t nitems, size_t size)
{
	void *mem = calloc(nitems, size);

//	printf("malloc 0x%08X, %d:  ", (uint32_t)mem, size);
//	pmdDoHeapCheck(82);

	if(!mem)
	{
		// No point in going on
		// Need to come back and add something to aid post-mortem analysis
		// Maybe printf will still work...
		printf("\r\nERROR:  calloc failed!\r\n");
#ifndef RELEASE_BUILD
		while(1);
#endif
		NVIC_SystemReset();
	}
//	printf("[MRem %d]", pmdGetHeapRemaining());

	return mem;
}

void pmdFree(void *ptr)
{
	free(ptr);
	app_malloc_times--;
}
#endif

void pmdCheckHeapValid(void)
{
	printf("Starting heap test\r\n");
	struct mallinfo mi = mallinfo();
	printf("mallinfo arena:  %d\r\n", mi.arena);
	printf("mallinfo uordblks:  %d\r\n", mi.uordblks);
	printf("mallinfo fordblks:  %d\r\n", mi.fordblks);
	printf("Remaining heap memory:  %d\r\n", (ALLOCATED_HEAP_SIZE - mi.uordblks));
}

#if 0 // ZGy_Note: TBD what to do with this function
void SysTick_Handler(void)
{
	extern volatile uint32_t uwTick;
	++uwTick;
}
#endif

//void HAL_SYSTICK_Callback(void)
//{
//}
//
//bool TimedOut(uint32_t start, uint32_t period)
//{
//	volatile uint32_t tick = GetMsTicks();
//	return (ticks - start) > period;
//}


uint8_t Get_CRC8(uint8_t *data, uint16_t size)
{
	uint16_t i;

	CRC->CR = CRC_CR_POLYSIZE_1 | CRC_CR_RESET;
	CRC->INIT = 0x00;
	CRC->POL = 0x9B;

	for(i=0; i<size; ++i)
	{
		*(uint8_t volatile*) (&CRC->DR) = data[i];
	}

	return CRC->DR;
}

uint16_t Get_CRC16_CCITT_On_Bytes(uint8_t *data, uint16_t size)
{
	uint16_t i;

	CRC->CR = CRC_CR_POLYSIZE_0 | CRC_CR_RESET;
	CRC->INIT = 0x00;
	CRC->POL = 0x1021;

	for(i=0; i<size; ++i)
	{
		*(uint8_t volatile*) (&CRC->DR) = data[i];
	}

	return CRC->DR;
}


uint16_t Get_CRC16_CCITT(uint16_t *data, uint16_t size_hw)
{
	uint16_t in;
	uint16_t i;

	CRC->CR = CRC_CR_POLYSIZE_0 | CRC_CR_RESET;
	CRC->INIT = 0xFFFF;
	CRC->POL = 0x1021;

	for(i=0; i<size_hw; ++i)
	{
		in = data[i];
		*(uint16_t volatile*) (&CRC->DR) = (in >> 8) | ((in & 0xFF) << 8);
	}

	return CRC->DR;
}

uint16_t Continue_CRC16_CCITT(uint16_t *data, uint16_t size_hw)
{
	uint16_t in;
	uint16_t i;

	for(i=0; i<size_hw; ++i)
	{
		in = data[i];
		*(uint16_t volatile*) (&CRC->DR) = (in >> 8) | ((in & 0xFF) << 8);
	}

	return CRC->DR;
}

int noprintf(const char *format, ...){return 0;}

static uint32_t PriMaskCache = 0;

void CritSect_Push()
{
	//if(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)	Use this for F767
	if(!(__get_IPSR() & 0x1F))			// Make sure we're not in an interrupt
	{
		PriMaskCache = __get_PRIMASK();

		if(PriMaskCache)
		{
			__disable_irq();
		}
	}
}

void CritSect_Pop()
{
	if(!(__get_IPSR() & 0x1F) && !PriMaskCache)
	{
		__enable_irq();
	}
}

void LogResetReason(uint32_t rcc_csr)
{
	printf("\r\nReset Reason\r\n|----------------------------------------------------|\r\n");
	if(rcc_csr & (1<<31)) printf("|  Low-power reset                                   |\r\n");
	if(rcc_csr & (1<<30)) printf("|  Window watchdog reset                             |\r\n");
	if(rcc_csr & (1<<29)) printf("|  Independent watchdog reset                        |\r\n");
	if(rcc_csr & (1<<28)) printf("|  Software reset                                    |\r\n");
	if(rcc_csr & (1<<27)) printf("|  Power-on/Power-down reset                         |\r\n");
	if(rcc_csr & (1<<26)) printf("|  Pin reset                                         |\r\n");
	if(rcc_csr & (1<<25)) printf("|  Brown out reset                                   |\r\n");
	if(rcc_csr & (1<<19)) printf("|  RTC software reset                                |\r\n");
	printf("|----------------------------------------------------|\r\n\r\n");
}
