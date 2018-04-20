/**
 * @file uart.c
 * @brief This file contains the source code for using the uart.
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/12/2018
 */
#include "uart.h"
#include "lcd.h"
// #include "button.h"


/// Sets all necessary registers to enable the uart 1 module
/** This method initializes all necessary registers for the uart.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void uart_init(void)
{

    //enable clock to GPIO, R1 = port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    //enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx(See DataSheet)
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;

    //enable alternate functions on port b pins 0 and 1
    GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);

    //enable Rx and Txon port B on pins 0 and 1
    GPIO_PORTB_PCTL_R |= 0x00000011;

    //set pin 0 and 1 to digital
    GPIO_PORTB_DEN_R |= (BIT0 | BIT1);

    //set pin 0 to Rx or input
    GPIO_PORTB_DIR_R &= ~BIT0;

    //set pin 1 to Txor output
    GPIO_PORTB_DIR_R |= BIT1;

    //calculate baudrate
    uint16_t iBRD= 8;

    //use equations
    uint16_t fBRD= 44;

    //use equations
    //turn off uart1 while we set it up
    UART1_CTL_R &= ~(UART_CTL_UARTEN);

    //set baud rate
    UART1_IBRD_R = iBRD;
    UART1_FBRD_R = fBRD;

    //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
    UART1_LCRH_R = UART_LCRH_WLEN_8 ;

    //use system clock as source
    UART1_CC_R = UART_CC_CS_SYSCLK;

    //re-enable enable RX, TX, and uart1
    UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);

} // END of uart_init()

/// Sends a character to Putty.
/** This method sends a single 8 bit character over the uart 1 module.
 * @param data the data to be sent out over uart 1
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void uart_sendChar(char data)
{

    //wait until there is room to send data
    while(UART1_FR_R & 0x20){

    }

    //send data
    UART1_DR_R = data;

}

/// Receives character
/** This method uses polling to receive an 8 bit character over uart 1 module.
 * @return the character received or a -1 if error occurred
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
int uart_receive(void)
{

    char data = 0;

     //&& !button_pressed()

    //wait to receive
    while(UART1_FR_R & UART_FR_RXFE){

    }

    // Get and return data
    data = (char)(UART1_DR_R & 0xFF);
    return data;

}

/// Sends string through uart
/** This method sends an entire string of character over uart 1 module
 * @param data pointer to the first index of the string to be sent
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void uart_sendStr(const char *data)
{
    
    //wait until there is room to send data
    while (data[0] != '\0') {
        uart_sendChar(data[0]);
        data++;
    }

}
