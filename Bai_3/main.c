#include "stm32f10x.h"

/*
 * ============================================================
 * TUAN 3 - BAI 3
 *
 * Yeu cau:
 * - UART
 * - Nut nhan
 * - DMA
 *
 * Moi lan nhan nut:
 * D23DTVM02N08:BTN:<gia_tri>\n\r
 *
 * USART1:
 * PA9  -> TX
 * PA10 -> RX
 * Baudrate = 9600
 * PCLK2 = 8 MHz
 * BRR = 0x0341
 *
 * DMA:
 * DMA1 Channel 4 -> USART1_TX
 * ============================================================
 */


/* ============================================================
 * DMA1 Channel 4 - dia chi thanh ghi
 *
 * DMA1 base        = 0x40020000
 *
 * ISR              = 0x40020000
 * IFCR             = 0x40020004
 *
 * Channel 4:
 * CCR4             = 0x40020008 + 3 * 0x14
 *                  = 0x40020044
 *
 * CNDTR4           = 0x40020048
 * CPAR4            = 0x4002004C
 * CMAR4            = 0x40020050
 * ============================================================
 */

#define DMA1_ISR_REG       (*(volatile unsigned int *)0x40020000)
#define DMA1_IFCR_REG      (*(volatile unsigned int *)0x40020004)

#define DMA1_CCR4_REG      (*(volatile unsigned int *)0x40020044)
#define DMA1_CNDTR4_REG    (*(volatile unsigned int *)0x40020048)
#define DMA1_CPAR4_REG     (*(volatile unsigned int *)0x4002004C)
#define DMA1_CMAR4_REG     (*(volatile unsigned int *)0x40020050)


/* ============================================================
 * Bien
 * ============================================================
 */

/* So lan nhan nut */
volatile unsigned int button_value = 0;

/* DMA dang gui = 1 */
volatile unsigned int dma_busy = 0;

/* Buffer chua ban tin UART */
char uart_buffer[40];


/* ============================================================
 * IRQ handlers
 * ============================================================
 */

void SysTick_Handler(void)
{
}

void EXTI0_IRQHandler(void)
{
}


/* ============================================================
 * DMA1 Channel 4 IRQ
 *
 * DMA1 Channel 4:
 * IRQ = DMA1_Channel4_IRQHandler
 * ============================================================
 */

void DMA1_Channel4_IRQHandler(void)
{
    /*
     * Kiem tra Transfer Complete Flag cua Channel 4.
     *
     * ISR:
     * bit 13 = TCIF4
     */
    if (DMA1_ISR_REG & (1 << 13))
    {
        /*
         * Tat DMA Channel 4
         *
         * CCR4 bit 0 = EN
         */
        DMA1_CCR4_REG &= ~(1 << 0);

        /*
         * Xoa co Transfer Complete
         */
        DMA1_IFCR_REG = (1 << 13);

        /*
         * DMA da gui xong
         */
        dma_busy = 0;
    }
}


/* ============================================================
 * Delay
 *
 * Chi dung cho debounce nut nhan.
 * Khong dung de cho UART.
 * ============================================================
 */

void Delay(volatile unsigned int time)
{
    while (time--)
    {
        __asm volatile ("nop");
    }
}


/* ============================================================
 * Chuyen so sang chuoi
 *
 * Vi du:
 *
 * 0   -> "0"
 * 1   -> "1"
 * 12  -> "12"
 * 123 -> "123"
 * ============================================================
 */

int NumberToString(unsigned int number, char *buffer)
{
    char temp[10];

    int i = 0;
    int j = 0;


    if (number == 0)
    {
        buffer[0] = '0';
        return 1;
    }


    while (number > 0)
    {
        temp[i++] = '0' + (number % 10);

        number = number / 10;
    }


    while (i > 0)
    {
        buffer[j++] = temp[--i];
    }


    return j;
}


/* ============================================================
 * Tao ban tin
 *
 * D23DTVM02N08:BTN:<value>\n\r
 * ============================================================
 */

int MakeMessage(unsigned int value, char *buffer)
{
    const char class_id[] = "D23DTMT02";
    const char group_id[] = "N01";

    int index = 0;
    int i;


    /* ID lop */
    for (i = 0; class_id[i] != '\0'; i++)
    {
        buffer[index++] = class_id[i];
    }


    /* ID nhom */
    for (i = 0; group_id[i] != '\0'; i++)
    {
        buffer[index++] = group_id[i];
    }


    /* :BTN: */
    buffer[index++] = ':';
    buffer[index++] = 'B';
    buffer[index++] = 'T';
    buffer[index++] = 'N';
    buffer[index++] = ':';


    /* Gia tri nut nhan */
    index += NumberToString(value, &buffer[index]);


    /* Ket thuc ban tin */
    buffer[index++] = '\n';
    buffer[index++] = '\r';


    return index;
}


/* ============================================================
 * GPIO INIT
 * ============================================================
 */

void GPIO_Init(void)
{
    /*
     * Bat clock GPIOA
     */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;


    /*
     * Bat clock GPIOC
     */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;


    /* ========================================================
     * PA0 - INPUT PULL-UP
     *
     * MODE = 00
     * CNF  = 10
     *
     * => 1000b = 0x8
     * ========================================================
     */

    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |=  (0x8 << 0);


    /*
     * Pull-up:
     *
     * ODR0 = 1
     */
    GPIOA->ODR |= (1 << 0);


    /* ========================================================
     * PC13 - OUTPUT PUSH PULL
     *
     * MODE = 10
     * CNF  = 00
     *
     * => 0010b = 0x2
     * ========================================================
     */

    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |=  (0x2 << 20);


    /*
     * PC13 = 1
     *
     * LED onboard OFF
     */
    GPIOC->BSRR = (1 << 13);


    /* ========================================================
     * PA9 - USART1 TX
     *
     * MODE = 11
     * CNF  = 10
     *
     * => 1011b = 0xB
     * ========================================================
     */

    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);


    /* ========================================================
     * PA10 - USART1 RX
     *
     * MODE = 00
     * CNF  = 01
     *
     * => 0100b = 0x4
     * ========================================================
     */

    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |=  (0x4 << 8);
}


/* ============================================================
 * USART1 INIT
 * ============================================================
 */

void UART1_Init(void)
{
    /*
     * Bat clock USART1
     */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;


    /*
     * Reset cau hinh USART
     */
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;


    /*
     * PCLK2 = 8 MHz
     * Baudrate = 9600
     *
     * BRR = 0x0341
     */
    USART1->BRR = 0x0341;


    /*
     * TE = 1
     * Transmitter Enable
     */
    USART1->CR1 |= (1 << 3);


    /*
     * RE = 1
     * Receiver Enable
     */
    USART1->CR1 |= (1 << 2);


    /*
     * UE = 1
     * USART Enable
     */
    USART1->CR1 |= (1 << 13);
}


/* ============================================================
 * DMA1 CHANNEL 4 INIT
 *
 * USART1_TX -> DMA1 Channel 4
 * ============================================================
 */

void DMA1_Channel4_Init(void)
{
    /*
     * Bat clock DMA1
     *
     * RCC AHBENR bit 0 = DMA1EN
     */
    RCC->AHBENR |= (1 << 0);


    /*
     * Tat Channel 4
     *
     * CCR4 bit 0 = EN
     */
    DMA1_CCR4_REG &= ~(1 << 0);


    /*
     * Xoa tat ca co cua Channel 4
     *
     * GIF4  = bit 12
     * TCIF4 = bit 13
     * HTIF4 = bit 14
     * TEIF4 = bit 15
     */
    DMA1_IFCR_REG =
          (1 << 12)
        | (1 << 13)
        | (1 << 14)
        | (1 << 15);


    /*
     * Xoa cau hinh cu
     */
    DMA1_CCR4_REG = 0;


    /*
     * TCIE = 1
     *
     * Enable interrupt khi DMA truyen xong
     *
     * CCR4 bit 1
     */
    DMA1_CCR4_REG |= (1 << 1);


    /*
     * DIR = 1
     *
     * Memory -> Peripheral
     *
     * CCR4 bit 4
     */
    DMA1_CCR4_REG |= (1 << 4);


    /*
     * MINC = 1
     *
     * Tang dia chi bo nho sau moi byte
     *
     * CCR4 bit 7
     */
    DMA1_CCR4_REG |= (1 << 7);


    /*
     * PSIZE = 00
     *
     * Peripheral size = 8 bit
     */


    /*
     * MSIZE = 00
     *
     * Memory size = 8 bit
     */


    /*
     * PL = 10
     *
     * Priority = High
     */
    DMA1_CCR4_REG |= (2 << 12);


    /*
     * Dia chi peripheral
     *
     * USART1->DR
     */
    DMA1_CPAR4_REG = (unsigned int)&USART1->DR;


    /*
     * USART1 cho phep DMA TX request
     *
     * USART_CR3 bit 7 = DMAT
     */
    USART1->CR3 |= (1 << 7);


    /*
     * Enable IRQ DMA1 Channel 4
     *
     * NVIC ISER0
     * Address = 0xE000E100
     *
     * DMA1 Channel 4 IRQ number = 14
     */
    *(volatile unsigned int *)0xE000E100 = (1 << 14);
}


/* ============================================================
 * UART1 DMA SEND
 *
 * Khong cho TXE.
 * Khong gui tung byte bang CPU.
 *
 * DMA se tu dong:
 *
 * uart_buffer[0]
 * uart_buffer[1]
 * uart_buffer[2]
 * ...
 *
 * -> USART1->DR
 * ============================================================
 */

void UART1_DMA_Send(char *buffer, unsigned int length)
{
    /*
     * Neu DMA dang gui thi khong gui ban tin moi.
     */
    if (dma_busy)
    {
        return;
    }


    /*
     * Bao DMA dang gui
     */
    dma_busy = 1;


    /*
     * Tat Channel 4 truoc khi cau hinh
     */
    DMA1_CCR4_REG &= ~(1 << 0);


    /*
     * Xoa co DMA
     */
    DMA1_IFCR_REG =
          (1 << 12)
        | (1 << 13)
        | (1 << 14)
        | (1 << 15);


    /*
     * Dia chi buffer trong RAM
     */
    DMA1_CMAR4_REG = (unsigned int)buffer;


    /*
     * So byte can gui
     */
    DMA1_CNDTR4_REG = length;


    /*
     * Bat DMA Channel 4
     */
    DMA1_CCR4_REG |= (1 << 0);
}


/* ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    unsigned int message_length;


    /*
     * Khoi tao bien
     */
    button_value = 0;
    dma_busy = 0;


    /*
     * GPIO
     */
    GPIO_Init();


    /*
     * UART
     */
    UART1_Init();


    /*
     * DMA
     */
    DMA1_Channel4_Init();


    /*
     * Vong lap
     */
    while (1)
    {
        /*
         * PA0 = 0 -> nut dang nhan
         */
        if (!(GPIOA->IDR & (1 << 0)))
        {
            /*
             * Debounce
             */
            Delay(50000);


            /*
             * Kiem tra lai nut
             */
            if (!(GPIOA->IDR & (1 << 0)))
            {
                /*
                 * Moi lan nhan tang 1
                 */
                button_value++;


                /*
                 * Bat LED PC13
                 *
                 * PC13 = 0 -> ON
                 */
                GPIOC->BSRR = (1 << 29);


                /*
                 * Tao ban tin
                 *
                 * D23DTVM02N08:BTN:<value>\n\r
                 */
                message_length =
                    MakeMessage(
                        button_value,
                        uart_buffer
                    );


                /*
                 * Gui ban tin bang DMA
                 */
                UART1_DMA_Send(
                    uart_buffer,
                    message_length
                );


                /*
                 * Cho nha nut
                 *
                 * Muc dich:
                 * 1 lan nhan = 1 lan tang
                 */
                while (!(GPIOA->IDR & (1 << 0)))
                {
                }


                /*
                 * Debounce luc nha nut
                 */
                Delay(50000);


                /*
                 * Tat LED
                 */
                GPIOC->BSRR = (1 << 13);
            }
        }
    }
}
