#include <compiler.h>
#include <stdint.h>
#include <stm32f10x.h>

static uint8_t initial_stack[1024];

extern void _start(void);

extern void stm32_USART1_IRQ(void);

const void * const __SECTION(".text.boot") vectab[] =
{
	/* arm exceptions */
	initial_stack + sizeof(initial_stack),
	_start,
	0, // nmi
	0, // hard fault
	0, // mem manage
	0, // bus fault
	0, // usage fault
	0, // reserved
	0, // reserved
	0, // reserved
	0, // reserved
	0, // svcall
	0, // debug monitor
	0, // reserved
	0, // pendsv
	0, // systick

	/* hardware irqs */
	[USART1_IRQn + 16] = stm32_USART1_IRQ, 
	[NUM_IRQn + 16] = 0,
};
