
#include <stdint.h>
#include <stm32.h>
#include <stdarg.h>

void _dputc(char c)
{
	if (c == '\n') {
		_dputc('\r');
	}
	while ((usart1->SR & (1<<7)) == 0)
		;
	usart1->DR = c & 0xff;
}

int dgetc(char *c, bool wait)
{
	return -1;
}

