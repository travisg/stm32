#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>

#include <stm32.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <core_cm3.h>

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
	USART_Cmd(USART1, ENABLE);
}

static void init_leds(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

	GPIO_InitTypeDef init;
	init.GPIO_Pin = GPIO_Pin_6;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_Init(GPIOF, &init);
	init.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOF, &init);
	init.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOF, &init);
	init.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF, &init);
}

static void set_led(uint led, bool en)
{
	uint16_t pin;

	switch (led) {
		default:
		case 0: pin = GPIO_Pin_6; break;
		case 1: pin = GPIO_Pin_7; break;
		case 2: pin = GPIO_Pin_8; break;
		case 3: pin = GPIO_Pin_9; break;
	}

	GPIO_WriteBit(GPIOF, pin, en ? Bit_SET : Bit_RESET);
}

static void dump_clocks(void)
{
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	printf("SYSCLKFrequency %u\n", clocks.SYSCLK_Frequency);
	printf("HCLKFrequency %u\n", clocks.HCLK_Frequency);
	printf("PCLK1Frequency %u\n", clocks.PCLK1_Frequency);
	printf("PCLK2Frequency %u\n", clocks.PCLK2_Frequency);
	printf("ADCCLKFrequency %u\n", clocks.ADCCLK_Frequency);
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

	/* configure the usart1 pins */
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

	init_leds();

	usart1_init();

	printf("how are you gentlemen\n");

	dump_clocks();

	// try to program up the pll
	printf("enabling pll\n");
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
	RCC_PLLCmd(ENABLE);

	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		;
	printf("pll latched\n");

	dump_clocks();

	printf("setting sysclk to pll\n");

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	usart1_init();

	set_led(3, 0);
	set_led(3, 1);
	printf("after new sysclk\n");

	dump_clocks();

	uint32_t val;
	for (val = 0; ; val++) {
		set_led(0, val & 0x1);
//		set_led(1, val & 0x2);
//		set_led(2, val & 0x4);
//		set_led(3, val & 0x8);
	}
}
