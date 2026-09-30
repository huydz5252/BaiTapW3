#include <stdint.h>

/* main() nằm trong main.c */
extern int main(void);

/* Địa chỉ đỉnh stack do linker cung cấp */
extern uint32_t _estack;

/* Các interrupt handler được sử dụng */
void Reset_Handler(void);
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);

/* Handler mặc định */
void Default_Handler(void);

/* Các handler không sử dụng */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));

void WWDG_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));

void EXTI1_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));

void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

void ADC1_2_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));

void EXTI9_5_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));

void TIM1_BRK_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));

void I2C1_EV_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));

void SPI1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));

void USART1_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));

void EXTI15_10_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));


/* Vector table */
__attribute__((section(".isr_vector"), used))
const void *vector_table[] =
{
    (void *)&_estack,              /* 0  Initial Stack Pointer */

    Reset_Handler,                 /* 1  Reset */
    NMI_Handler,                   /* 2  NMI */
    HardFault_Handler,             /* 3  HardFault */
    MemManage_Handler,             /* 4  MemManage */
    BusFault_Handler,              /* 5  BusFault */
    UsageFault_Handler,            /* 6  UsageFault */

    0,                              /* 7  Reserved */
    0,                              /* 8  Reserved */
    0,                              /* 9  Reserved */
    0,                              /* 10 Reserved */

    SVC_Handler,                   /* 11 SVC */
    DebugMon_Handler,              /* 12 Debug Monitor */

    0,                              /* 13 Reserved */

    PendSV_Handler,                /* 14 PendSV */
    SysTick_Handler,               /* 15 SysTick */

    WWDG_IRQHandler,               /* 16 */
    PVD_IRQHandler,                /* 17 */
    TAMPER_IRQHandler,             /* 18 */
    RTC_IRQHandler,                /* 19 */
    FLASH_IRQHandler,              /* 20 */
    RCC_IRQHandler,                /* 21 */

    EXTI0_IRQHandler,              /* 22 */
    EXTI1_IRQHandler,              /* 23 */
    EXTI2_IRQHandler,              /* 24 */
    EXTI3_IRQHandler,              /* 25 */
    EXTI4_IRQHandler,              /* 26 */

    DMA1_Channel1_IRQHandler,      /* 27 */
    DMA1_Channel2_IRQHandler,      /* 28 */
    DMA1_Channel3_IRQHandler,      /* 29 */
    DMA1_Channel4_IRQHandler,      /* 30 */
    DMA1_Channel5_IRQHandler,      /* 31 */
    DMA1_Channel6_IRQHandler,      /* 32 */
    DMA1_Channel7_IRQHandler,      /* 33 */

    ADC1_2_IRQHandler,              /* 34 */
    USB_HP_CAN1_TX_IRQHandler,      /* 35 */
    USB_LP_CAN1_RX0_IRQHandler,     /* 36 */
    CAN1_RX1_IRQHandler,            /* 37 */
    CAN1_SCE_IRQHandler,            /* 38 */

    EXTI9_5_IRQHandler,             /* 39 */

    TIM1_BRK_IRQHandler,            /* 40 */
    TIM1_UP_IRQHandler,             /* 41 */
    TIM1_TRG_COM_IRQHandler,        /* 42 */
    TIM1_CC_IRQHandler,             /* 43 */

    TIM2_IRQHandler,                /* 44 */
    TIM3_IRQHandler,                /* 45 */
    TIM4_IRQHandler,                /* 46 */

    I2C1_EV_IRQHandler,             /* 47 */
    I2C1_ER_IRQHandler,             /* 48 */
    I2C2_EV_IRQHandler,             /* 49 */
    I2C2_ER_IRQHandler,             /* 50 */

    SPI1_IRQHandler,                /* 51 */
    SPI2_IRQHandler,                /* 52 */

    USART1_IRQHandler,              /* 53 */
    USART2_IRQHandler,              /* 54 */
    USART3_IRQHandler,              /* 55 */

    EXTI15_10_IRQHandler,           /* 56 */
    RTCAlarm_IRQHandler,            /* 57 */
    USBWakeUp_IRQHandler            /* 58 */
};


/* Reset Handler */
void Reset_Handler(void)
{
    main();

    while (1)
    {
    }
}


/* Handler mặc định */
void Default_Handler(void)
{
    while (1)
    {
    }
}
