#include "stm32f10x.h"

#define BUFFER_SIZE 1
uint16_t adc_value;

/* Khởi tạo UART1 (PA9: TX, PA10: RX) ở tốc độ 115200 */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    // PA9: Alternate function push-pull (TX)
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);

    // PA10: Input floating (RX)
    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |=  (0x4 << 8);

    // Baudrate 115200 với PCLK2 = 8MHz
    USART1->BRR = 0x0341;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/* Gửi một chuỗi ký tự qua UART1 */
void UART1_SendString(char *s)
{
    while (*s)
    {
        while (!(USART1->SR & (1 << 7))); // Chờ TXE trống
        USART1->DR = *s++;
    }
}

/* Chuyển đổi số nguyên thành chuỗi để in ra Serial */
void UART1_SendNumber(uint16_t num)
{
    char buf[10];
    int i = 0;
    if (num == 0)
    {
        UART1_SendString("0\r\n");
        return;
    }
    while (num > 0)
    {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    // In ngược lại cho đúng thứ tự
    while (i > 0)
    {
        while (!(USART1->SR & (1 << 7)));
        USART1->DR = buf[--i];
    }
    UART1_SendString("\r\n");
}

/* Khởi tạo Timer3 làm nguồn Trigger cho ADC (tần số 100Hz) */
void TIM3_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Timer clock = 8MHz. Chia tần PSC = 79 => Timer clock = 100kHz
    TIM3->PSC = 7999;
    // ARR = 999 => Tần số tràn = 100kHz / 1000 = 100Hz
    TIM3->ARR = 999;

    // Cấu hình TRGO (Master Mode Selection) chọn Update event làm trigger
    TIM3->CR2 &= ~TIM_CR2_MMS;
    TIM3->CR2 |=  (2UL << 4); // MMS = 010 (Update)

    // Bật Timer3
    TIM3->CR1 |= TIM_CR1_CEN;
}

/* Khởi tạo ADC1 đọc kênh PA0, kích hoạt bằng Timer3 TRGO */
void ADC1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

    // PA0: Analog input
    GPIOA->CRL &= ~(0xF << 0);

    // Cấu hình ADC1
    ADC1->CR2 |= ADC_CR2_ADON;  // Bật ADC
    for (volatile int i = 0; i < 1000; i++); // Delay nhỏ ổn định

    // Hiệu chuẩn ADC
    ADC1->CR2 |= (1 << 3); // RSTCAL
    while (ADC1->CR2 & (1 << 3));
    ADC1->CR2 |= (1 << 2); // CAL
    while (ADC1->CR2 & (1 << 2));

    // Chọn kênh 0 (PA0) làm chuỗi chuyển đổi đầu tiên (SQR3)
    ADC1->SQR3 = 0;

    // Kích hoạt bên ngoài bằng Timer3 TRGO và bật External Trigger Conversion
    ADC1->CR2 &= ~(7UL << 17); // Xóa EXTSEL
    ADC1->CR2 |=  (4UL << 17); // EXTSEL = 100 (TIM3 TRGO event cho ADC1)
    ADC1->CR2 |=  ADC_CR2_EXTTRIG;

    // Bật DMA cho ADC
    ADC1->CR2 |= ADC_CR2_DMA;

    // Khởi động lại ADC sau cùng
    ADC1->CR2 |= ADC_CR2_ADON;
}

/* Khởi tạo DMA1 Channel 1 cho ADC1 */
void DMA1_Init(void)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Địa chỉ ngoại vi (ADC1 Data Register)
    DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
    // Địa chỉ bộ nhớ (Biến lưu giá trị ADC)
    DMA1_Channel1->CMAR = (uint32_t)&adc_value;
    // Số lượng dữ liệu (1 mẫu)
    DMA1_Channel1->CNDTR = BUFFER_SIZE;

    // Cấu hình: Hoạt động chế độ Circular, tăng địa chỉ bộ nhớ, hướng Peripheral-to-Memory, bật ngắt Transfer Complete
    DMA1_Channel1->CCR = DMA_CCR1_CIRC | DMA_CCR1_MINC | DMA_CCR1_TCIE;

    // Cho phép ngắt DMA1 Channel 1 trong NVIC
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    // Bật DMA Channel 1
    DMA1_Channel1->CCR |= DMA_CCR1_EN;
}

/* Hàm ngắt DMA1 Channel 1 khi có dữ liệu mới từ ADC */
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA1->ISR & (1 << 1)) // Kiểm tra cờ Transfer Complete của Channel 1
    {
        DMA1->IFCR |= (1 << 1); // Xóa cờ ngắt

        // Gửi giá trị đọc được từ ADC qua UART
        UART1_SendString("ADC Value: ");
        UART1_SendNumber(adc_value);
    }
}

/* Các hàm ngắt trống để tránh lỗi Linker */
void DMA1_Channel4_IRQHandler(void) {}
//void SysTick_Handler(void) {}
//void EXTI0_IRQHandler(void) {}

int main(void)
{
    UART1_Init();
    ADC1_Init();
    TIM3_Init();
    DMA1_Init();

    // Bắt đầu chuyển đổi ADC bằng phần mềm lần đầu
    ADC1->CR2 |= (1 << 22); // SWSTART

    while (1)
    {
        // Vòng lặp chính thảnh thơi vì toàn bộ quá trình đọc và gửi đã có Timer và DMA lo tự động
    }
}
