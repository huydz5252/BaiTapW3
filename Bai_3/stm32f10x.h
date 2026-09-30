#ifndef STM32F10X_H
#define STM32F10X_H

#include <stdint.h>

/* =========================================================
   CORE
   ========================================================= */

#define SCS_BASE            (0xE000E000UL)
#define SYSTICK_BASE        (SCS_BASE + 0x0010UL)
#define NVIC_BASE           (SCS_BASE + 0x0100UL)

typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_Type;

#define SysTick ((SysTick_Type *)SYSTICK_BASE)

typedef enum
{
    WWDG_IRQn = 0,
    PVD_IRQn = 1,
    TAMPER_IRQn = 2,
    RTC_IRQn = 3,
    FLASH_IRQn = 4,
    RCC_IRQn = 5,
    EXTI0_IRQn = 6,
    EXTI1_IRQn = 7,
    EXTI2_IRQn = 8,
    EXTI3_IRQn = 9,
    EXTI4_IRQn = 10,
    DMA1_Channel1_IRQn = 11,
    DMA1_Channel2_IRQn = 12,
    DMA1_Channel3_IRQn = 13,
    DMA1_Channel4_IRQn = 14,
    DMA1_Channel5_IRQn = 15,
    DMA1_Channel6_IRQn = 16,
    DMA1_Channel7_IRQn = 17,
    ADC1_2_IRQn = 18,
    USB_HP_CAN1_TX_IRQn = 19,
    USB_LP_CAN1_RX0_IRQn = 20,
    CAN1_RX1_IRQn = 21,
    CAN1_SCE_IRQn = 22,
    EXTI9_5_IRQn = 23,
    TIM1_BRK_IRQn = 24,
    TIM1_UP_IRQn = 25,
    TIM1_TRG_COM_IRQn = 26,
    TIM1_CC_IRQn = 27,
    TIM2_IRQn = 28,
    TIM3_IRQn = 29,
    TIM4_IRQn = 30,
    I2C1_EV_IRQn = 31,
    I2C1_ER_IRQn = 32,
    I2C2_EV_IRQn = 33,
    I2C2_ER_IRQn = 34,
    SPI1_IRQn = 35,
    SPI2_IRQn = 36,
    USART1_IRQn = 37,
    USART2_IRQn = 38,
    USART3_IRQn = 39,
    EXTI15_10_IRQn = 40
} IRQn_Type;

typedef struct
{
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
} NVIC_Type;

#define NVIC ((NVIC_Type *)NVIC_BASE)

static inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    NVIC->ISER[IRQn >> 5] =
        (1UL << (IRQn & 0x1F));
}

#define SysTick_CTRL_ENABLE_Msk        (1UL << 0)
#define SysTick_CTRL_TICKINT_Msk       (1UL << 1)
#define SysTick_CTRL_CLKSOURCE_Msk     (1UL << 2)

static inline uint32_t SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1UL) > 0xFFFFFFUL)
        return 1;

    SysTick->LOAD = ticks - 1UL;
    SysTick->VAL = 0;

    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk |
        SysTick_CTRL_TICKINT_Msk |
        SysTick_CTRL_ENABLE_Msk;

    return 0;
}

/* =========================================================
   RCC
   ========================================================= */

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

#define RCC_BASE            0x40021000UL
#define RCC                 ((RCC_TypeDef *)RCC_BASE)

#define RCC_AHBENR_DMA1EN       (1UL << 0)

#define RCC_APB2ENR_AFIOEN      (1UL << 0)
#define RCC_APB2ENR_IOPAEN      (1UL << 2)
#define RCC_APB2ENR_IOPBEN      (1UL << 3)
#define RCC_APB2ENR_IOPCEN      (1UL << 4)
#define RCC_APB2ENR_USART1EN    (1UL << 14)

/* =========================================================
   GPIO
   ========================================================= */

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

#define GPIOA_BASE          0x40010800UL
#define GPIOB_BASE          0x40010C00UL
#define GPIOC_BASE          0x40011000UL

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

/* PA0 */
#define GPIO_CRL_MODE0         (3UL << 0)
#define GPIO_CRL_CNF0          (3UL << 2)
#define GPIO_CRL_CNF0_1        (1UL << 3)

/* PA9 */
#define GPIO_CRH_MODE9         (3UL << 4)
#define GPIO_CRH_CNF9          (3UL << 6)
#define GPIO_CRH_MODE9_0       (1UL << 4)
#define GPIO_CRH_MODE9_1       (1UL << 5)
#define GPIO_CRH_CNF9_1        (1UL << 7)

/* PA10 */
#define GPIO_CRH_MODE10        (3UL << 8)
#define GPIO_CRH_CNF10         (3UL << 10)
#define GPIO_CRH_CNF10_0       (1UL << 10)

#define GPIO_ODR_ODR0          (1UL << 0)

/* =========================================================
   AFIO
   ========================================================= */

typedef struct
{
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t MAPR2;
} AFIO_TypeDef;

#define AFIO_BASE           0x40010000UL
#define AFIO                ((AFIO_TypeDef *)AFIO_BASE)

#define AFIO_EXTICR1_EXTI0     (0xFUL << 0)

/* =========================================================
   EXTI
   ========================================================= */

typedef struct
{
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} EXTI_TypeDef;

#define EXTI_BASE           0x40010400UL
#define EXTI                ((EXTI_TypeDef *)EXTI_BASE)

#define EXTI_IMR_MR0        (1UL << 0)
#define EXTI_FTSR_TR0       (1UL << 0)
#define EXTI_PR_PR0         (1UL << 0)

/* =========================================================
   USART1
   ========================================================= */

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

#define USART1_BASE         0x40013800UL
#define USART1              ((USART_TypeDef *)USART1_BASE)

#define USART_CR1_UE        (1UL << 13)
#define USART_CR1_TE        (1UL << 3)
#define USART_CR1_RE        (1UL << 2)

#define USART_CR3_DMAT      (1UL << 7)

/* =========================================================
   DMA
   ========================================================= */

typedef struct
{
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
    uint32_t RESERVED;
} DMA_Channel_TypeDef;

typedef struct
{
    volatile uint32_t ISR;
    volatile uint32_t IFCR;

    DMA_Channel_TypeDef Channel1;
    DMA_Channel_TypeDef Channel2;
    DMA_Channel_TypeDef Channel3;
    DMA_Channel_TypeDef Channel4;
    DMA_Channel_TypeDef Channel5;
    DMA_Channel_TypeDef Channel6;
    DMA_Channel_TypeDef Channel7;
} DMA_TypeDef;

#define DMA1_BASE           0x40020000UL
#define DMA1                ((DMA_TypeDef *)DMA1_BASE)

#define DMA1_Channel1       (&DMA1->Channel1)
#define DMA1_Channel2       (&DMA1->Channel2)
#define DMA1_Channel3       (&DMA1->Channel3)
#define DMA1_Channel4       (&DMA1->Channel4)
#define DMA1_Channel5       (&DMA1->Channel5)
#define DMA1_Channel6       (&DMA1->Channel6)
#define DMA1_Channel7       (&DMA1->Channel7)

/* DMA Channel 4 CCR */
#define DMA_CCR4_EN         (1UL << 0)
#define DMA_CCR4_TCIE       (1UL << 1)
#define DMA_CCR4_DIR        (1UL << 4)
#define DMA_CCR4_MINC       (1UL << 7)

/* DMA Channel 4 flags */
#define DMA_ISR_TCIF4       (1UL << 13)

#define DMA_IFCR_CTCIF4     (1UL << 13)
#define DMA_IFCR_CGIF4      (1UL << 12)

#endif
