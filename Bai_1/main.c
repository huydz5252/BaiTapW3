#include "stm32f10x.h"
#include <stdint.h>

#define SSD1306_ADDR 0x3C

/* =========================================================
 * DELAY
 * ========================================================= */

void delay_ms(uint32_t ms)
{
    uint32_t i;
    uint32_t j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __asm volatile ("nop");
        }
    }
}

/* =========================================================
 * I2C1 GPIO
 *
 * PB6 = SCL
 * PB7 = SDA
 * ========================================================= */

void I2C1_GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    GPIOB->CRL &= ~(
        GPIO_CRL_MODE6 |
        GPIO_CRL_CNF6  |
        GPIO_CRL_MODE7 |
        GPIO_CRL_CNF7
    );

    /*
     * Alternate Function Open Drain
     * Output 50 MHz
     */

    GPIOB->CRL |=
        GPIO_CRL_MODE6_0 |
        GPIO_CRL_MODE6_1 |
        GPIO_CRL_CNF6_0  |
        GPIO_CRL_CNF6_1  |
        GPIO_CRL_MODE7_0 |
        GPIO_CRL_MODE7_1 |
        GPIO_CRL_CNF7_0  |
        GPIO_CRL_CNF7_1;
}

/* =========================================================
 * I2C1 INIT
 *
 * APB1 = 36 MHz
 * I2C = 100 kHz
 * ========================================================= */

void I2C1_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    I2C1->CR2 = 36;
    I2C1->CCR = 180;
    I2C1->TRISE = 37;

    I2C1->CR1 = I2C_CR1_PE;
}

/* =========================================================
 * I2C START
 * ========================================================= */

void I2C1_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;

    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
    }
}

/* =========================================================
 * I2C SEND ADDRESS
 * ========================================================= */

void I2C1_SendAddress(uint8_t address)
{
    I2C1->DR = address << 1;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
    }

    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

/* =========================================================
 * I2C WRITE BYTE
 * ========================================================= */

void I2C1_WriteByte(uint8_t data)
{
    I2C1->DR = data;

    while (!(I2C1->SR1 & I2C_SR1_TXE))
    {
    }
}

/* =========================================================
 * I2C STOP
 * ========================================================= */

void I2C1_Stop(void)
{
    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
    }

    I2C1->CR1 |= I2C_CR1_STOP;
}

/* =========================================================
 * SSD1306 COMMAND
 * ========================================================= */

void SSD1306_WriteCommand(uint8_t command)
{
    I2C1_Start();

    I2C1_SendAddress(SSD1306_ADDR);

    /* 0x00 = command */
    I2C1_WriteByte(0x00);

    I2C1_WriteByte(command);

    I2C1_Stop();
}

/* =========================================================
 * SSD1306 INIT
 * ========================================================= */

void SSD1306_Init(void)
{
    delay_ms(100);

    SSD1306_WriteCommand(0xAE);

    SSD1306_WriteCommand(0xD5);
    SSD1306_WriteCommand(0x80);

    SSD1306_WriteCommand(0xA8);
    SSD1306_WriteCommand(0x3F);

    SSD1306_WriteCommand(0xD3);
    SSD1306_WriteCommand(0x00);

    SSD1306_WriteCommand(0x40);

    SSD1306_WriteCommand(0x8D);
    SSD1306_WriteCommand(0x14);

    SSD1306_WriteCommand(0x20);
    SSD1306_WriteCommand(0x00);

    SSD1306_WriteCommand(0xA1);

    SSD1306_WriteCommand(0xC8);

    SSD1306_WriteCommand(0xDA);
    SSD1306_WriteCommand(0x12);

    SSD1306_WriteCommand(0x81);
    SSD1306_WriteCommand(0x7F);

    SSD1306_WriteCommand(0xD9);
    SSD1306_WriteCommand(0xF1);

    SSD1306_WriteCommand(0xDB);
    SSD1306_WriteCommand(0x40);

    SSD1306_WriteCommand(0xA4);

    SSD1306_WriteCommand(0xA6);

    SSD1306_WriteCommand(0xAF);

    delay_ms(100);
}

/* =========================================================
 * SET PAGE + COLUMN
 * ========================================================= */

void SSD1306_SetPosition(uint8_t page, uint8_t column)
{
    SSD1306_WriteCommand(0xB0 | page);

    SSD1306_WriteCommand(0x00 | (column & 0x0F));

    SSD1306_WriteCommand(0x10 | (column >> 4));
}

/* =========================================================
 * GHI 1 BYTE DATA
 * ========================================================= */

void SSD1306_WriteDataByte(uint8_t data)
{
    I2C1_Start();

    I2C1_SendAddress(SSD1306_ADDR);

    /* 0x40 = display data */
    I2C1_WriteByte(0x40);

    I2C1_WriteByte(data);

    I2C1_Stop();
}

/* =========================================================
 * CLEAR OLED
 * ========================================================= */

void SSD1306_Clear(void)
{
    uint8_t page;
    uint8_t column;

    for (page = 0; page < 8; page++)
    {
        SSD1306_SetPosition(page, 0);

        for (column = 0; column < 128; column++)
        {
            SSD1306_WriteDataByte(0x00);
        }
    }
}

/* =========================================================
 * FONT 5x7
 *
 * Mỗi ký tự gồm 5 cột.
 *
 * Bit = 1: pixel sáng
 * Bit = 0: pixel tắt
 * ========================================================= */

const uint8_t FONT_P[5] =
{
    0x7F,
    0x09,
    0x09,
    0x09,
    0x06
};

const uint8_t FONT_T[5] =
{
    0x01,
    0x01,
    0x7F,
    0x01,
    0x01
};

const uint8_t FONT_I[5] =
{
    0x00,
    0x41,
    0x7F,
    0x41,
    0x00
};

/* =========================================================
 * VẼ 1 KÝ TỰ
 *
 * page = 3
 * column = vị trí bắt đầu
 * ========================================================= */

void SSD1306_DrawChar(uint8_t page,
                      uint8_t column,
                      const uint8_t *font)
{
    uint8_t i;

    SSD1306_SetPosition(page, column);

    for (i = 0; i < 5; i++)
    {
        SSD1306_WriteDataByte(font[i]);
    }

    /* Khoảng cách giữa các chữ */
    SSD1306_WriteDataByte(0x00);
}

/* =========================================================
 * HIỂN THỊ PTIT
 * ========================================================= */

void SSD1306_Show_PTIT(void)
{
    /*
     * OLED 128x64
     *
     * Đặt chữ ở khoảng giữa màn hình.
     *
     * P = column 44
     * T = column 50
     * I = column 56
     * T = column 62
     */

    SSD1306_DrawChar(3, 44, FONT_P);

    SSD1306_DrawChar(3, 50, FONT_T);

    SSD1306_DrawChar(3, 56, FONT_I);

    SSD1306_DrawChar(3, 62, FONT_T);
}

/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    /* Khởi tạo GPIO I2C */
    I2C1_GPIO_Init();

    /* Khởi tạo I2C1 */
    I2C1_Init();

    /* Khởi tạo OLED */
    SSD1306_Init();

    /* Xóa màn hình */
    SSD1306_Clear();

    /* Hiển thị chữ PTIT */
    SSD1306_Show_PTIT();

    /* Giữ màn hình */
    while (1)
    {
    }
}
