#include "stm32f10x.h"

/* =========================================================
   STM32F103 REGISTERS
   ========================================================= */

#define RCC_APB2ENR   (*(volatile unsigned long *)0x40021018)

#define GPIOA_CRL     (*(volatile unsigned long *)0x40010800)
#define GPIOA_BSRR    (*(volatile unsigned long *)0x40010810)
#define GPIOA_BRR     (*(volatile unsigned long *)0x40010814)

#define USART1_SR     (*(volatile unsigned long *)0x40013800)
#define USART1_DR     (*(volatile unsigned long *)0x40013804)
#define USART1_BRR    (*(volatile unsigned long *)0x40013808)
#define USART1_CR1    (*(volatile unsigned long *)0x4001380C)

#define SPI1_CR1      (*(volatile unsigned long *)0x40013000)
#define SPI1_CR2      (*(volatile unsigned long *)0x40013004)
#define SPI1_SR       (*(volatile unsigned long *)0x40013008)
#define SPI1_DR       (*(volatile unsigned long *)0x4001300C)


/* =========================================================
   CLOCK ENABLE
   ========================================================= */

void Clock_Init(void)
{
    /*
       Bit 2  = IOPAEN
       Bit 12 = SPI1EN
       Bit 14 = USART1EN
    */

    RCC_APB2ENR |= (1 << 2);
    RCC_APB2ENR |= (1 << 12);
    RCC_APB2ENR |= (1 << 14);
}


/* =========================================================
   UART1
   PA9  = TX
   PA10 = RX
   ========================================================= */

void UART1_Init(void)
{
    /*
       PA9:
       MODE9 = 11  -> Output 50MHz
       CNF9  = 10  -> Alternate function push-pull

       PA10:
       MODE10 = 00 -> Input
       CNF10  = 01 -> Floating input
    */

    /* Xóa cấu hình PA9 */
    GPIOA_CRL = GPIOA_CRL;

    /*
       PA9 và PA10 nằm trong CRH,
       nên dùng địa chỉ CRH = 0x40010804
    */

    #define GPIOA_CRH (*(volatile unsigned long *)0x40010804)

    /* Xóa PA9 và PA10 */
    GPIOA_CRH &= ~((0xF << 4) | (0xF << 8));

    /* PA9 = 1110
       MODE9 = 11
       CNF9  = 10
    */
    GPIOA_CRH |= (0xB << 4);

    /* PA10 = 0100
       MODE10 = 00
       CNF10 = 01
    */
    GPIOA_CRH |= (0x4 << 8);


    /*
       Baudrate 9600
       APB2 = 72MHz
       USARTDIV = 72MHz / (16*9600)
       BRR = 0x1D4C
    */

    USART1_BRR = 0x0341;

    /*
       UE  = bit 13
       TE  = bit 3
       RE  = bit 2
    */

    USART1_CR1 = (1 << 13) |
                 (1 << 3)  |
                 (1 << 2);
}


/* =========================================================
   UART SEND CHAR
   TXE = bit 7
   ========================================================= */

void UART1_SendChar(char c)
{
    while (!(USART1_SR & (1 << 7)))
    {
    }

    USART1_DR = c;
}


/* =========================================================
   UART SEND STRING
   ========================================================= */

void UART1_SendString(char *str)
{
    while (*str)
    {
        UART1_SendChar(*str);
        str++;
    }
}


/* =========================================================
   UART SEND HEX
   ========================================================= */

void UART1_SendHex(unsigned char data)
{
    char hex[] = "0123456789ABCDEF";

    UART1_SendChar(hex[(data >> 4) & 0x0F]);
    UART1_SendChar(hex[data & 0x0F]);
}


/* =========================================================
   SPI1
   PA4 = CS
   PA5 = SCK
   PA6 = MISO
   PA7 = MOSI
   ========================================================= */

void SPI1_Init(void)
{
    /* PA4 = CS: output push-pull 50MHz */
    GPIOA_CRL &= ~(0xF << 16);
    GPIOA_CRL |=  (0x3 << 16);

    /* PA5 = SCK: alternate function push-pull 50MHz */
    GPIOA_CRL &= ~(0xF << 20);
    GPIOA_CRL |=  (0xB << 20);

    /* PA6 = MISO: input floating */
    GPIOA_CRL &= ~(0xF << 24);
    GPIOA_CRL |=  (0x4 << 24);

    /* PA7 = MOSI: alternate function push-pull 50MHz */
    GPIOA_CRL &= ~(0xF << 28);
    GPIOA_CRL |=  (0xB << 28);

    /* CS HIGH khi chưa giao tiếp */
    GPIOA_BSRR = (1 << 4);

    /*
       SPI1:
       Master
       CPOL = 0
       CPHA = 0
       MSB first
       Baudrate = PCLK2 / 16
       Software NSS
    */

    SPI1_CR1 = 0;

    SPI1_CR1 |= (1 << 2);       /* MSTR */
    SPI1_CR1 |= (3 << 3);       /* BR = 011 -> /16 */
    SPI1_CR1 |= (1 << 8);       /* SSI */
    SPI1_CR1 |= (1 << 9);       /* SSM */

    /* Enable SPI */
    SPI1_CR1 |= (1 << 6);
}

unsigned char SPI1_Transfer(unsigned char data)
{
    while (!(SPI1_SR & (1 << 1)))
    {
    }

    SPI1_DR = data;

    while (!(SPI1_SR & (1 << 0)))
    {
    }

    return (unsigned char)SPI1_DR;
}


/* =========================================================
   W25Q64 CHIP SELECT
   ========================================================= */

void W25Q64_CS_Low(void)
{
    GPIOA_BRR = (1 << 4);
}


void W25Q64_CS_High(void)
{
    GPIOA_BSRR = (1 << 4);
}


/* =========================================================
   READ JEDEC ID
   Command = 0x9F
   ========================================================= */

void W25Q64_ReadID(unsigned char *manufacturer,
                   unsigned char *memory_type,
                   unsigned char *capacity)
{
    W25Q64_CS_Low();

    SPI1_Transfer(0x9F);

    *manufacturer = SPI1_Transfer(0xFF);
    *memory_type  = SPI1_Transfer(0xFF);
    *capacity     = SPI1_Transfer(0xFF);

    W25Q64_CS_High();
}


/* =========================================================
   READ STATUS REGISTER
   Command = 0x05
   ========================================================= */

unsigned char W25Q64_ReadStatus(void)
{
    unsigned char status;

    W25Q64_CS_Low();

    SPI1_Transfer(0x05);

    status = SPI1_Transfer(0xFF);

    W25Q64_CS_High();

    return status;
}


/* =========================================================
   WRITE ENABLE
   Command = 0x06
   ========================================================= */

void W25Q64_WriteEnable(void)
{
    W25Q64_CS_Low();

    SPI1_Transfer(0x06);

    W25Q64_CS_High();
}


/* =========================================================
   WAIT BUSY
   BUSY = bit 0
   ========================================================= */

void W25Q64_WaitBusy(void)
{
    while (W25Q64_ReadStatus() & 0x01)
    {
    }
}


/* =========================================================
   SECTOR ERASE
   Command = 0x20
   Size = 4KB
   ========================================================= */

void W25Q64_SectorErase(unsigned long address)
{
    W25Q64_WriteEnable();

    W25Q64_CS_Low();

    SPI1_Transfer(0x20);

    SPI1_Transfer((address >> 16) & 0xFF);
    SPI1_Transfer((address >> 8)  & 0xFF);
    SPI1_Transfer(address & 0xFF);

    W25Q64_CS_High();

    W25Q64_WaitBusy();
}


/* =========================================================
   PAGE PROGRAM
   Command = 0x02
   ========================================================= */

void W25Q64_PageProgram(unsigned long address,
                        unsigned char *data,
                        unsigned int length)
{
    unsigned int i;

    W25Q64_WriteEnable();

    W25Q64_CS_Low();

    SPI1_Transfer(0x02);

    SPI1_Transfer((address >> 16) & 0xFF);
    SPI1_Transfer((address >> 8)  & 0xFF);
    SPI1_Transfer(address & 0xFF);

    for (i = 0; i < length; i++)
    {
        SPI1_Transfer(data[i]);
    }

    W25Q64_CS_High();

    W25Q64_WaitBusy();
}


/* =========================================================
   READ DATA
   Command = 0x03
   ========================================================= */

void W25Q64_Read(unsigned long address,
                 unsigned char *data,
                 unsigned int length)
{
    unsigned int i;

    W25Q64_CS_Low();

    SPI1_Transfer(0x03);

    SPI1_Transfer((address >> 16) & 0xFF);
    SPI1_Transfer((address >> 8)  & 0xFF);
    SPI1_Transfer(address & 0xFF);

    for (i = 0; i < length; i++)
    {
        data[i] = SPI1_Transfer(0xFF);
    }

    W25Q64_CS_High();
}


/* =========================================================
   MAIN
   ========================================================= */

void SysTick_Handler(void)
{
}

void EXTI0_IRQHandler(void)
{
}

void DMA1_Channel4_IRQHandler(void)
{
}

int main(void)
{
    unsigned char manufacturer;
    unsigned char memory_type;
    unsigned char capacity;

    unsigned char write_data[] = "PTIT W25Q64";
    unsigned char read_data[12];

    unsigned int i;


    /* Clock */
    Clock_Init();

    /* UART */
    UART1_Init();

    /* SPI */
    SPI1_Init();


    UART1_SendString("\r\n");
    UART1_SendString("============================\r\n");
    UART1_SendString("STM32F103 + W25Q64\r\n");
    UART1_SendString("============================\r\n");


    /* ==========================================
       READ ID
       ========================================== */

    UART1_SendString("Read JEDEC ID...\r\n");

    W25Q64_ReadID(&manufacturer,
                  &memory_type,
                  &capacity);

    UART1_SendString("ID = ");

    UART1_SendHex(manufacturer);
    UART1_SendChar(' ');

    UART1_SendHex(memory_type);
    UART1_SendChar(' ');

    UART1_SendHex(capacity);

    UART1_SendString("\r\n");


    /* ==========================================
       ERASE
       ========================================== */

    UART1_SendString("Erase sector...\r\n");

    W25Q64_SectorErase(0x000000);

    UART1_SendString("Erase OK\r\n");


    /* ==========================================
       WRITE
       ========================================== */

    UART1_SendString("Write data: ");
    UART1_SendString((char *)write_data);
    UART1_SendString("\r\n");

    W25Q64_PageProgram(0x000000,
                       write_data,
                       11);

    UART1_SendString("Write OK\r\n");


    /* ==========================================
       READ
       ========================================== */

    UART1_SendString("Read data...\r\n");

    W25Q64_Read(0x000000,
                read_data,
                11);

    read_data[11] = '\0';

    UART1_SendString("Data = ");

    for (i = 0; i < 11; i++)
    {
        UART1_SendChar(read_data[i]);
    }

    UART1_SendString("\r\n");


    /* ==========================================
       COMPARE
       ========================================== */

    for (i = 0; i < 11; i++)
    {
        if (read_data[i] != write_data[i])
        {
            UART1_SendString("RESULT: ERROR\r\n");

            while (1)
            {
            }
        }
    }

    UART1_SendString("RESULT: PASS\r\n");


    while (1)
    {
    }
}
