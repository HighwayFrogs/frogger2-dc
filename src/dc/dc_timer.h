#include <shinobi.h>

#define NUM_TIMERS	10

typedef struct
{
	char	ident[32];
	Uint32	start;
	Uint32	count;
	Uint32	micro;
}DCTIMER;

extern DCTIMER	timers[NUM_TIMERS];

void InitTimerbar();

void DCTIMER_RESET();

void DCTIMER_START(int num);

void DCTIMER_STOP_ADD(int num);

void DCTIMER_STOP(int num);

void DCTIMER_PRINTS();
