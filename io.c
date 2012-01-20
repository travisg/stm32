
#include <stdint.h>
#include <stm32.h>
#include <stdarg.h>

#include <stm32f10x_usart.h>

void _dputc(char c)
{
	if (c == '\n') {
		_dputc('\r');
	}
#if 0
	while ((usart1->SR & (1<<7)) == 0)
		;
	usart1->DR = c & 0xff;
#else
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0)
		;
	USART_SendData(USART1, c);
#endif
}

int dgetc(char *c, bool wait)
{
	return -1;
}

