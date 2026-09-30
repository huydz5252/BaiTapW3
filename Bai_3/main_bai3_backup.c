#include "stm32f10x.h"

/* =========================
   THÔNG TIN LỚP / NHÓM
   ========================= */
#define ID_LOP  "D23DTVM02"
#define ID_NHOM "N08"

/* =========================
   BIẾN TOÀN CỤC
   ========================= */
volatile uint32_t systick_ms = 0;
volatile uint32_t button_value = 0;
volatile uint8_t dma_busy = 0;

volatile char tx_buffer[50];

/* =========================
   SYSYTick 1 ms
   ========================= */
void SysTick_Handler(void)
{
    systick_ms++;
}

/* =========================
   LẤY THỜI GIAN ms
   ========================= */
uint32_t millis(void)
{
    return systick_ms;
}

/* =========================
   UART1
   PA9  -> TX
   PA10 -> RX
   9600 baud
   PCLK2 = 8 MHz
   BRR = 0x0341
   ========================= */
void UART1_Init(void)
{
    /* Clock GPIOA + AFIO + USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* PA9 - USART1_TX
       Alternate Function Push-Pull
       50 MHz
    */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_0 |
                  GPIO_CRH_MODE9_1 |
                  GPIO_CRH_CNF9_1;

    /* PA10 - USART1_RX
       Input floating
    */
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    /* 8 MHz / 9600 baud */
    USART1->BRR = 0x0341;

    /* Enable USART, TX, RX */
    USART1->CR1 = USART_CR1_UE |
                  USART_CR1_TE |
                  USART_CR1_RE;
}

/* =========================
   DMA1 CHANNEL 4
   USART1_TX = DMA1 Channel 4
   ========================= */
void DMA_USART1_TX_Init(void)
{
    /* Clock DMA1 */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    /* Disable Channel 4 */
    DMA1_Channel4->CCR &= ~DMA_CCR4_EN;

    /* Xóa cờ DMA */
    DMA1->IFCR = DMA_IFCR_CGIF4;

    /*
       DIR  = 1  : Memory -> Peripheral
       MINC = 1  : tăng địa chỉ RAM
       PSIZE = 8 bit
       MSIZE = 8 bit
       TCIE = 1  : ngắt khi truyền xong
    */
    DMA1_Channel4->CCR =
        DMA_CCR4_DIR |
        DMA_CCR4_MINC |
        DMA_CCR4_TCIE;

    /* Địa chỉ USART1->DR */
    DMA1_Channel4->CPAR =
        (uint32_t)&USART1->DR;

    /* Cho phép USART1 phát dữ liệu bằng DMA */
    USART1->CR3 |= USART_CR3_DMAT;

    /* Cho phép ngắt DMA Channel 4 */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

/* =========================
   GỬI DỮ LIỆU BẰNG DMA
   ========================= */
void UART1_DMA_Send(char *data, uint16_t length)
{
    /* DMA đang bận */
    if (dma_busy)
        return;

    dma_busy = 1;

    /* Tắt DMA trước khi cấu hình */
    DMA1_Channel4->CCR &= ~DMA_CCR4_EN;

    /* Xóa cờ DMA */
    DMA1->IFCR = DMA_IFCR_CGIF4;

    /* Địa chỉ buffer */
    DMA1_Channel4->CMAR = (uint32_t)data;

    /* Số byte cần truyền */
    DMA1_Channel4->CNDTR = length;

    /* Bật DMA */
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}

/* =========================
   UINT32 -> STRING
   ========================= */
uint16_t uint_to_string(uint32_t number, char *str)
{
    char temp[12];
    uint16_t i = 0;
    uint16_t j = 0;

    if (number == 0)
    {
        str[0] = '0';
        return 1;
    }

    while (number > 0)
    {
        temp[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0)
    {
        str[j++] = temp[--i];
    }

    return j;
}

/* =========================
   TẠO BẢN TIN
   D23DTVM02N08:BTN:1\n\r
   ========================= */
uint16_t Create_Message(uint32_t value)
{
    uint16_t index = 0;

    /* ID lớp */
    tx_buffer[index++] = 'D';
    tx_buffer[index++] = '2';
    tx_buffer[index++] = '3';
    tx_buffer[index++] = 'D';
    tx_buffer[index++] = 'T';
    tx_buffer[index++] = 'V';
    tx_buffer[index++] = 'M';
    tx_buffer[index++] = '0';
    tx_buffer[index++] = '2';

    /* ID nhóm */
    tx_buffer[index++] = 'N';
    tx_buffer[index++] = '0';
    tx_buffer[index++] = '8';

    /* :BTN: */
    tx_buffer[index++] = ':';
    tx_buffer[index++] = 'B';
    tx_buffer[index++] = 'T';
    tx_buffer[index++] = 'N';
    tx_buffer[index++] = ':';

    /* Giá trị nút nhấn */
    index += uint_to_string(value,
                            (char *)&tx_buffer[index]);

    /* \n\r */
    tx_buffer[index++] = '\n';
    tx_buffer[index++] = '\r';

    return index;
}

/* =========================
   NÚT NHẤN PA0
   PA0 = INPUT PULL-UP
   ========================= */
void Button_Init(void)
{
    /* Clock GPIOA */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA0 Input Pull-up */
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 |
                    GPIO_CRL_CNF0);

    GPIOA->CRL |= GPIO_CRL_CNF0_1;

    /* Pull-up */
    GPIOA->ODR |= GPIO_ODR_ODR0;

    /* Clock AFIO */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* EXTI0 dùng PA0 */
    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;

    /* Cho phép EXTI0 */
    EXTI->IMR |= EXTI_IMR_MR0;

    /* Ngắt cạnh xuống */
    EXTI->FTSR |= EXTI_FTSR_TR0;

    /* Enable NVIC EXTI0 */
    NVIC_EnableIRQ(EXTI0_IRQn);
}

/* =========================
   NGẮT NÚT NHẤN
   ========================= */
void EXTI0_IRQHandler(void)
{
    static uint32_t last_press = 0;
    uint32_t now;

    /* Kiểm tra EXTI0 */
    if (EXTI->PR & EXTI_PR_PR0)
    {
        /* Xóa cờ ngắt */
        EXTI->PR = EXTI_PR_PR0;

        now = millis();

        /* Chống dội 50 ms */
        if ((now - last_press) >= 50)
        {
            last_press = now;

            /* Tăng giá trị */
            button_value++;

            /* Tạo bản tin */
            uint16_t len =
                Create_Message(button_value);

            /* Gửi bằng DMA */
            UART1_DMA_Send(
                (char *)tx_buffer,
                len
            );
        }
    }
}

/* =========================
   DMA1 CHANNEL 4 IRQ
   ========================= */
void DMA1_Channel4_IRQHandler(void)
{
    /* Kiểm tra truyền xong */
    if (DMA1->ISR & DMA_ISR_TCIF4)
    {
        /* Xóa cờ Transfer Complete */
        DMA1->IFCR = DMA_IFCR_CTCIF4;

        /* Tắt DMA */
        DMA1_Channel4->CCR &= ~DMA_CCR4_EN;

        /* DMA đã rảnh */
        dma_busy = 0;
    }
}

/* =========================
   MAIN
   ========================= */
int main(void)
{
    /* SysTick 1 ms
       SystemCoreClock = 8 MHz
    */
    SysTick_Config(8000);

    /* UART1 */
    UART1_Init();

    /* DMA USART1 TX */
    DMA_USART1_TX_Init();

    /* Button */
    Button_Init();

    while (1)
    {
        /*
           CPU không cần chờ UART.

           Nhấn nút
              ↓
           EXTI0
              ↓
           button_value++
              ↓
           tạo message
              ↓
           DMA
              ↓
           USART1
              ↓
           PA9
        */
    }
}
