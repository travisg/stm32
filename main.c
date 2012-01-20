#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>

#include <stm32.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

/* externals */
extern unsigned int __data_start_rom, __data_start, __data_end;
extern unsigned int __bss_start, __bss_end;

int somedata = 99;
int somedata2 = 100;
int somebssdata[3];

void stm32_USART1_IRQ(void)
{
	printf("USART1_IRQ\n");
}

static void usart1_init(void)
{
#if 0
	/* configure usart1 */
	usart1->CR1 = (0<<13) | (1<<3) | (0<<2);
	usart1->BRR = (4<<4)|(5<<0);
	usart1->CR2 = 0;
	usart1->CR3 = 0;

	usart1->CR1 |= (1<<13); // start the uart
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitTypeDef init;

	init.USART_BaudRate = 115200;
	init.USART_WordLength = USART_WordLength_8b;
	init.USART_StopBits = USART_StopBits_1;
	init.USART_Parity = USART_Parity_No;
	init.USART_Mode = USART_Mode_Tx;
	init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1, &init);

	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
#endif
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

void _start(void)
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
//	stm32_clock_set_enable(CLOCK_USART1, true);
//	stm32_clock_set_enable(CLOCK_GPIOA, true);
//	stm32_clock_set_enable(CLOCK_AFIO, true);

	/* configure the usart1 pins */
#if 0
	// USART1 is connected to GPIOA_pin9, GPIOA_pin10
	afio->MAPR &= ~(1<<2); // clear USART1 remap
	RMW(gpioa->CRH, 4, 4, (2 << 2)|(3 << 0)); // pin 9, alternate function output push-pull, 50mhz
	RMW(gpioa->CRH, 8, 4, (1 << 2)|(0 << 0)); // pin 10, input floating
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);

	GPIO_InitTypeDef init;
	init.GPIO_Pin = GPIO_Pin_9;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &init);

	init.GPIO_Pin = GPIO_Pin_10;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &init);
#endif

	usart1_init();

	printf("RCC regs:\n");
	hexdump(rcc, sizeof(*rcc));

	for (;;)
		;
}
