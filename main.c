#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>

#include <stm32.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_dbgmcu.h>
#include <misc.h>
#include <core_cm3.h>

/* externals */
extern unsigned int __data_start_rom, __data_start, __data_end;
extern unsigned int __bss_start, __bss_end;

void _nmi(void)
{
	printf("nmi\n");
	halt();
}

void _hardfault(void)
{
	printf("hardfault\n");
	halt();
}

void _memmanage(void)
{
	printf("memmanage\n");
	halt();
}

void _busfault(void)
{
	printf("busfault\n");
	halt();
}

void _usagefault(void)
{
	printf("usagefault\n");
	halt();
}

void _svc(void)
{
	printf("svc\n");
	halt();
}

void _pendsv(void)
{
	printf("pendsv\n");
	halt();
}

volatile uint32_t tick_counter = 0;
const uint32_t systick_counter = 10000000;
const uint32_t systick_rate = 80000000;

void _systick(void)
{
	tick_counter++; 
}

uint32_t current_time(void)
{
	do {
		uint32_t t = tick_counter;
		uint32_t delta = SysTick->VAL;
		uint32_t t2 = tick_counter;
		if (t2 != t)
			continue;

		uint32_t time = (t2 * systick_counter + (systick_counter - delta)) / (systick_rate / 1000000);

		return time;
	} while (true);
}

void spin(uint32_t usecs)
{
	uint32_t t = current_time();

	while ((current_time() - t) < usecs)
		;
}

void stm32_USART1_IRQ(void)
{
	printf("USART1_IRQ\n");
}

static void usart_init(USART_TypeDef *usart)
{
	if (usart == USART1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	} else if (usart == USART2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	} else if (usart == USART3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}

	USART_InitTypeDef init;

	init.USART_BaudRate = 115200;
	init.USART_WordLength = USART_WordLength_8b;
	init.USART_StopBits = USART_StopBits_1;
	init.USART_Parity = USART_Parity_No;
	init.USART_Mode = USART_Mode_Tx;
	init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(usart, &init);

	USART_ITConfig(usart, USART_IT_RXNE, DISABLE);
	USART_Cmd(usart, ENABLE);
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

#define LED_CS GPIO_Pin_6
#define LED_WR GPIO_Pin_7
#define LED_DATA GPIO_Pin_8

void led_panel_write(uint32_t dat, size_t len)
{
	uint i;
	for (i = len; i > 0; i--) {
		GPIO_ResetBits(GPIOF, LED_WR);
		GPIO_WriteBit(GPIOF, LED_DATA, (dat & (1<<(i-1))) ? SET : RESET);
		GPIO_SetBits(GPIOF, LED_WR);
	}
}

void led_panel_command_write(uint32_t cmd, size_t len)
{
	GPIO_ResetBits(GPIOF, LED_CS);

	led_panel_write(cmd, len);

	GPIO_SetBits(GPIOF, LED_CS);
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


	USART_TypeDef *debug_usart;
#if TARGET_STM3210E
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

	debug_usart = USART1;
#endif
#if TARGET_STM32_P107
	/* configure the usart3 pins */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);

	GPIO_InitTypeDef init;
	init.GPIO_Pin = GPIO_Pin_8;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &init);

	init.GPIO_Pin = GPIO_Pin_9;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &init);

	debug_usart = USART3;
#endif

	init_leds();

	usart_init(debug_usart);

	printf("how are you gentlemen\n");

	printf("devid 0x%x\n", DBGMCU_GetDEVID());

	dump_clocks();
	
	// bring up te HSE
	printf("enabling external crystal\n");
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_WaitForHSEStartUp();
	printf("external crystal up\n");

	// try to program up the pll
	printf("enabling pll\n");
#if STM32F10X_CL
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_4);
#else
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif
	RCC_PLLCmd(ENABLE);

	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		;
	printf("pll latched\n");

	printf("setting up clocks\n");

	FLASH_SetLatency(FLASH_Latency_2);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
#if STM32F10X_CL
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
#else
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
#endif

	usart_init(debug_usart);

	set_led(3, 0);
	set_led(3, 1);
	printf("after new sysclk\n");

	dump_clocks();

	printf("done!\n");

	/* try to fire the systick */
//	__set_BASEPRI(8 << __NVIC_PRIO_BITS);

	/* start the systick timer */
	NVIC_SetVectorTable(0, 0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	uint32_t pri = NVIC_EncodePriority(3, 0, 0);	
	NVIC_SetPriority(SysTick_IRQn, pri);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(systick_counter);

#if 0
	uint32_t val;
	for (val = 0; ; val++) {
		set_led(0, val & 0x1);
		set_led(1, val & 0x2);
		set_led(2, val & 0x4);
		set_led(3, val & 0x8);
	}
#endif

	/* write the boot sequence */
	led_panel_command_write(0b100000000010, 12); // SYS_EN
	led_panel_command_write(0b100000000110, 12); // LED_ON
	led_panel_command_write(0b100000010000, 12); // BLINK_OFF
	led_panel_command_write(0b100000110000, 12); // INT_RC
	led_panel_command_write(0b100001001000, 12); // n-mos open drain, 16 com
	led_panel_command_write(0b100101011110, 12); // PWM_CTRL | 0xf

	for(uint j = 0; ; j++) {
		GPIO_ResetBits(GPIOF, LED_CS);
		led_panel_write(0b1010000000, 10); // start write at address 0

		for (int i = 0; i < 96; i++) {
			led_panel_write(((j % 96) > i) ? 0b1111: 0, 4);
		}
		GPIO_SetBits(GPIOF, LED_CS);

		spin(10000);
	}

	for(;;)
		;
}

