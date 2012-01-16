#include <sys/types.h>
#include <stdint.h>
#include <stm32.h>

/* externals */
extern unsigned int __data_start_rom, __data_start, __data_end;
extern unsigned int __bss_start, __bss_end;

int somedata = 99;
int somedata2 = 100;
int somebssdata[3];

struct stm32_rcc_regs *rcc = (void *)RCC_BASE;
struct stm32_afio_regs *afio = (void *)AFIO_BASE;
struct stm32_gpio_regs *gpioa = (void *)GPIOA_BASE;
struct stm32_usart_regs *usart1 = (void *)USART1_BASE;

static void putchar(char c)
{
	while ((usart1->SR & (1<<7)) == 0)
		;
	usart1->DR = c & 0xff;
}

static void puts(const char *str)
{
	while (*str) {
		if (*str == '\n')
			putchar('\r');
		putchar(*str);
		str++;
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
	rcc->APB2ENR |= (1<<14)|(1<<2)|(1<<0); // USART1/GPIOA/AFIO

	/* configure the usart1 pins */
	afio->MAPR &= ~(1<<2); // clear USART1 remap

	RMW(gpioa->CRH, 4, 4, (2 << 2)|(3 << 0)); // pin 9, alternate function output push-pull, 50mhz
	RMW(gpioa->CRH, 8, 4, (1 << 2)|(0 << 0)); // pin 10, input floating

	// USART1 is connected to GPIOA_pin9, GPIOA_pin10

	/* write something out of the usart */
	usart1->CR1 = (0<<13) | (1<<3) | (0<<2);
	usart1->BRR = (4<<4)|(5<<0);
	usart1->CR2 = (0<<12);
	usart1->CR3 = 0;

	usart1->CR1 |= (1<<13); // start the uart

	for (;;) {
		puts("fucccccckkkkkkkk!\n");
	}
	
	for (;;)
		;
}
