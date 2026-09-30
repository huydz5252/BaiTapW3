#include "stm32f10x.h"
#include <stdint.h>

extern int main(void);

extern uint32_t _estack;

void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector")))
const uint32_t vector_table[] =
{
    (uint32_t)&_estack,

    (uint32_t)Reset_Handler,

    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,

    0,
    0,
    0,
    0,

    (uint32_t)SVC_Handler,
    (uint32_t)DebugMon_Handler,
    0,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler
};

void Reset_Handler(void)
{
    uint32_t *src;
    uint32_t *dst;

    /* Copy .data from Flash to RAM */
    extern uint32_t _sidata;
    extern uint32_t _sdata;
    extern uint32_t _edata;

    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /* Clear .bss */
    extern uint32_t _sbss;
    extern uint32_t _ebss;

    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    main();

    while (1)
    {
    }
}

void Default_Handler(void)
{
    while (1)
    {
    }
}
