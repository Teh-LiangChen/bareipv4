#include "stm32l5xx.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) {tick++;}

static void lpuart_init(void)
{
    // Enable clock for Power
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    (void) RCC->APB1ENR1;     // dummy read: wait for clock to start 

    // Enable IOSV to remove power Vddio2 isolation because it use different power source
    PWR->CR2 |= PWR_CR2_IOSV;

    // Enable clock for GPIOG 
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
    (void) RCC->AHB2ENR;     // dummy read: wait for clock to start 
    
    // Enable clock for LPUART1 
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
    (void) RCC->APB1ENR2;     // dummy read: wait for clock to start 

    //Set alternate function mode for PG7 which is AF8 to be LPUART1_TX
    GPIOG->AFR[0] &= ~(0b1111U << (7*4)); //clear bit
    GPIOG->AFR[0] |=  (0b1000U << (7*4)); //set bit to 8 

    //Set alternate function mode for PG8 which is AF8 to be LPUART1_RX
    GPIOG->AFR[1] &= ~(0b1111U); //clear bit
    GPIOG->AFR[1] |=  (0b1000U); //set bit to 8

    //Enable pull up on PG8 to avoid floating pin issue
    GPIOG->PUPDR &= ~(3U << (8*2));
    GPIOG->PUPDR |=  (1U << (8*2));

    //Set pin mode on PG7 and PG8 to alternate function mode which is 0b10
    GPIOG->MODER &= ~(0b1111U << (7*2)); //clear bit
    GPIOG->MODER |=  (0b1010U << (7*2)); //set bit to 10

    //set LPUART_BRR first since UE=0 after reset
    LPUART1->BRR = 106667U; //BRR = 256 x 4M / 115200 = 8889

    //set LPUART control register on the TE,RE,UE bit
    LPUART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);

    // LPUART1->CR1 &= ~(0b1101U); //clear bit on 0th (UE), 2nd (RE) and 3rd (TE)
    // LPUART1->CR1 |=  (0b1101U); //set bit on 0th (UE), 2nd (RE) and 3rd
     
}

static void lpuart_putc(char c)
{
    // while((LPUART1->ISR & 0b10000000) == 0) {}
    while (!(LPUART1->ISR & USART_ISR_TXE_TXFNF))
        LPUART1->TDR = c;
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
    lpuart_init();

    uint32_t last_tick = 0;
    static const uint32_t HEARTBEAT = 3000;

    while(1)
    {
        if ((tick - last_tick) >= HEARTBEAT)
        {
            last_tick = tick;
            lpuart_putc('A');
            lpuart_putc('B');
            lpuart_putc('C');
        }
    }
}