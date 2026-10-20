#include "stm32l5xx.h"

static void delay(volatile uint32_t n)
{
    while (n--) {__NOP(); }
}

static void lpuart_init(void)
{
    /* Enable clock for Power*/
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    (void) RCC->APB1ENR1;     /* dummy read: wait for clock to start */

    /* Enable IOSV to remove power Vddio2 isolation because it use different power source*/
    PWR->CR2 |= PWR_CR2_IOSV;

    /* Enable clock for GPIOG */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
    (void) RCC->AHB2ENR;     /* dummy read: wait for clock to start */
    
    /* Enable clock for LPUART1 */
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
    (void) RCC->APB1ENR2;     /* dummy read: wait for clock to start */

    /*Set alternate function mode for PG7 which is AF8 to be LPUART1_TX*/
    GPIOG->AFR[0] &= ~(0b1111U << (7*4)); //clear bit
    GPIOG->AFR[0] |=  (0b1000U << (7*4)); //set bit to 8

    /*Set alternate function mode for PG8 which is AF8 to be LPUART1_RX*/
    GPIOG->AFR[1] &= ~(0b1111U); //clear bit
    GPIOG->AFR[1] |=  (0b1000U); //set bit to 8

    /*Set pin mode on PG7 and PG8 to alternate function mode which is 0b10*/
    GPIOG->MODER &= ~(0b1111U << (7*2)); //clear bit
    GPIOG->MODER |=  (0b1010U << (7*2)); //set bit to 10

    //set LPUART_BRR first since UE=0 after reset
    LPUART1->BRR = 8889U; //BRR = 256 x 4M / 115200 = 8889 = 0x22b9

    //set LPUART control register on the TE,RE,UE bit
    LPUART1->CR1 &= ~(0b1101U); //clear bit on 0th (UE), 2nd (RE) and 3rd (TE) 
    // LPUART11->CR1 &= ~(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
    LPUART1->CR1 |=  (0b1101U); //set bit on 0th (UE), 2nd (RE) and 3rd
    // LPUART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE); 
}

static void lpuart_putc(char c)
{
    //while (!(LPUART1->ISR & USART_ISR_TXE_TXFNF))
    while((LPUART1->ISR & 0b10000000) == 0) {}
    LPUART1->TDR = c;
}

int main(void)
{
    lpuart_init();

    while(1)
    {
        lpuart_putc('A');
        delay(500000);
    }
}