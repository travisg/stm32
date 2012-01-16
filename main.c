#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>
#include <stm32.h>

/* externals */
extern unsigned int __data_start_rom, __data_start, __data_end;
extern unsigned int __bss_start, __bss_end;

int somedata = 99;
int somedata2 = 100;
int somebssdata[3];

static void usart1_init(void)
{
	/* configure usart1 */
	usart1->CR1 = (0<<13) | (1<<3) | (0<<2);
	usart1->BRR = (4<<4)|(5<<0);
	usart1->CR2 = 0;
	usart1->CR3 = 0;

	usart1->CR1 |= (1<<13); // start the uart
}

void stm32_clock_set_enable(uint clock, bool en)
{
	volatile uint32_t *reg;
	switch (clock & _CLOCK_BUS_MASK) {
		case _CLOCK_APB1:
			reg = &rcc->APB1ENR;
			break;
		case _CLOCK_APB2:
			reg = &rcc->APB2ENR;
			break;
		case _CLOCK_AHB:
			reg = &rcc->AHBENR;
			break;
		default:
			panic("bad clock 0x%x\n", clock);
			return;
	}
	if (en) {
		*reg |= 1 << (clock & _CLOCK_MASK);
	} else {
		*reg &= ~(1 << (clock & _CLOCK_MASK));
	}
}

void _main(void)
{
	/* copy data from rom */
	if (&__data_start != &__data_start_rom) {
		unsigned int *src = &__data_start_rom;
		unsigned int *dest = &__data_start;

		while (dest != &__data_end)
			*dest++ = *src++;
	}

	/* zero out bss */
	unsigned int *bss = &__bss_start;
	while (bss != &__bss_end)
		*bss++ = 0;

	/* clock the usart */
	stm32_clock_set_enable(CLOCK_USART1, true);
	stm32_clock_set_enable(CLOCK_GPIOA, true);
	stm32_clock_set_enable(CLOCK_AFIO, true);

	/* configure the usart1 pins */
	// USART1 is connected to GPIOA_pin9, GPIOA_pin10
	afio->MAPR &= ~(1<<2); // clear USART1 remap
	RMW(gpioa->CRH, 4, 4, (2 << 2)|(3 << 0)); // pin 9, alternate function output push-pull, 50mhz
	RMW(gpioa->CRH, 8, 4, (1 << 2)|(0 << 0)); // pin 10, input floating

	usart1_init();

	printf("RCC regs:\n");
	hexdump(rcc, sizeof(*rcc));

	for (;;)
		;
}
