#ifndef STM32F10X_H
#define STM32F10X_H

#include <stdint.h>

/* =========================

* Base addresses
* ========================= */

#define PERIPH_BASE       0x40000000UL
#define APB1PERIPH_BASE   PERIPH_BASE
#define APB2PERIPH_BASE   0x40010000UL

#define AHBPERIPH_BASE    0x40018000UL

/* =========================

* RCC
* ========================= */

#define RCC_BASE          0x40021000UL

typedef struct
{
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
} RCC_TypeDef;

#define RCC ((RCC_TypeDef *)RCC_BASE)

#define RCC_APB2ENR_IOPBEN       (1UL << 3)
#define RCC_APB1ENR_I2C1EN       (1UL << 21)

#define RCC_APB1RSTR_I2C1RST     (1UL << 21)

/* =========================

* GPIO
* ========================= */

#define GPIOB_BASE        0x40010C00UL

typedef struct
{
volatile uint32_t CRL;
volatile uint32_t CRH;
volatile uint32_t IDR;
volatile uint32_t ODR;
volatile uint32_t BSRR;
volatile uint32_t BRR;
volatile uint32_t LCKR;
} GPIO_TypeDef;

#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)

/* GPIO CRL bit definitions */

#define GPIO_CRL_MODE6       (0x3UL << 24)
#define GPIO_CRL_CNF6        (0x3UL << 26)

#define GPIO_CRL_MODE7       (0x3UL << 28)
#define GPIO_CRL_CNF7        (0x3UL << 30)

#define GPIO_CRL_MODE6_0     (1UL << 24)
#define GPIO_CRL_MODE6_1     (1UL << 25)

#define GPIO_CRL_CNF6_0      (1UL << 26)
#define GPIO_CRL_CNF6_1      (1UL << 27)

#define GPIO_CRL_MODE7_0     (1UL << 28)
#define GPIO_CRL_MODE7_1     (1UL << 29)

#define GPIO_CRL_CNF7_0      (1UL << 30)
#define GPIO_CRL_CNF7_1      (1UL << 31)

/* =========================

* I2C1
* ========================= */

#define I2C1_BASE        0x40005400UL

typedef struct
{
volatile uint32_t CR1;
volatile uint32_t CR2;
volatile uint32_t OAR1;
volatile uint32_t OAR2;
volatile uint32_t DR;
volatile uint32_t SR1;
volatile uint32_t SR2;
volatile uint32_t CCR;
volatile uint32_t TRISE;
volatile uint32_t FLTR;
} I2C_TypeDef;

#define I2C1 ((I2C_TypeDef *)I2C1_BASE)

/* I2C CR1 */

#define I2C_CR1_PE       (1UL << 0)
#define I2C_CR1_START    (1UL << 8)
#define I2C_CR1_STOP     (1UL << 9)

/* I2C SR1 */

#define I2C_SR1_SB       (1UL << 0)
#define I2C_SR1_ADDR     (1UL << 1)
#define I2C_SR1_BTF      (1UL << 2)
#define I2C_SR1_TXE      (1UL << 7)

#endif
