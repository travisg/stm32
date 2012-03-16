
#include <stdint.h>
#include <stm32.h>
#include <stdarg.h>

#include <stm32f10x_usart.h>

#if TARGET_STM3210E
#define DEBUG_USART USART1
#elif TARGET_STM32_P107
#define DEBUG_USART USART3
#else
#error define DEBUG_UART
#endif

void _dputc(char c)
{
	if (c == '\n') {
		_dputc('\r');
	}
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == 0)
		;
	USART_SendData(DEBUG_USART, c);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TC) == 0)
		;
}

int dgetc(char *c, bool wait)
{
	return -1;
}

