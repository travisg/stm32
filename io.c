
#include <stdint.h>
#include <stm32.h>
#include <stdarg.h>

#include <stm32f10x_usart.h>

void _dputc(char c)
{
	if (c == '\n') {
		_dputc('\r');
	}
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0)
		;
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == 0)
		;
}

int dgetc(char *c, bool wait)
{
	return -1;
}

