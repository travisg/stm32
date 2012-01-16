#pragma once

#include <reg.h>

/* stm32f10x defines */
#define TIM2_BASE	(0x40000000)
#define TIM3_BASE	(0x40000400)
#define TIM4_BASE	(0x40000800)
#define TIM5_BASE	(0x40000c00)
#define TIM6_BASE	(0x40001000)
#define TIM7_BASE	(0x40001400)
#define TIM12_BASE	(0x40001800)
#define TIM13_BASE	(0x40001c00)
#define TIM14_BASE	(0x40002000)
#define RTC_BASE	(0x40002800)
#define WWDG_BASE	(0x40002c00)
#define IWDG_BASE	(0x40003000)
#define SPI2_BASE	(0x40003800)
#define SPI3_BASE	(0x40003c00)
#define USART2_BASE	(0x40004400)
#define USART3_BASE	(0x40004800)
#define UART4_BASE	(0x40004c00)
#define UART5_BASE	(0x40005000)
#define I2C1_BASE	(0x40005400)
#define I2C2_BASE	(0x40005800)
#define USB_FS_BASE	(0x40005c00)
#define USB_CAN_RAM_BASE	(0x40006000)
#define BKP_BASE	(0x40006c00)
#define PWR_BASE	(0x40007000)
#define DAC_BASE	(0x40007400)
#define AFIO_BASE	(0x40010000)
#define EXTI_BASE	(0x40010400)
#define GPIOA_BASE	(0x40010800)
#define GPIOB_BASE	(0x40010c00)
#define GPIOC_BASE	(0x40011000)
#define GPIOD_BASE	(0x40011400)
#define GPIOE_BASE	(0x40011800)
#define GPIOF_BASE	(0x40011c00)
#define GPIOG_BASE	(0x40012000)
#define ADC1_BASE	(0x40012400)
#define ADC2_BASE	(0x40012800)
#define TIM1_BASE	(0x40012c00)
#define SPI1_BASE	(0x40013000)
#define TIM8_BASE	(0x40013400)
#define USART1_BASE	(0x40013800)
#define ADC3_BASE	(0x40013c00)
#define TIM9_BASE	(0x40014c00)
#define TIM10_BASE	(0x40015000)
#define TIM11_BASE	(0x40015400)
#define SDIO_BASE	(0x40018000)
#define DMA1_BASE	(0x40020000)
#define DMA2_BASE	(0x40020400)
#define RCC_BASE	(0x40021000)
#define FMI_BASE	(0x40022000)
#define CRC_BASE	(0x40023000)
#define ETH_BASE	(0x40028000)
#define USB_OTG_FS_BASE	(0x50000000)
#define FSMC_BASE	(0xa0000000)

#define FSMC_BASE	(0xa0000000)

/* RCC regs */
struct stm32_rcc_regs {
	volatile uint32_t CR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
};

/* GPIO regs */
struct stm32_gpio_regs {
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
};

struct stm32_afio_regs {
	volatile uint32_t EVCR;
	volatile uint32_t MAPR;
	volatile uint32_t EXTICR1;
	volatile uint32_t EXTICR2;
	volatile uint32_t EXTICR3;
	volatile uint32_t EXTICR4;
	volatile uint32_t MAPR2;
};

/* USART regs */
#define USART_SR	(0x00)
#define USART_DR	(0x04)
#define USART_BRR	(0x08)
#define USART_CR1	(0x0c)
#define USART_CR2	(0x10)
#define USART_CR3	(0x14)
#define USART_GTPR	(0x18)

struct stm32_usart_regs {
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
};

