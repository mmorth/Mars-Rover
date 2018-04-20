/**
 * @file pwm.c
 * @brief This file contains the source code for using pwm with the servo.
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/12/2018
 */

#include "Timer.h"
#include "lcd.h"
// #include "button.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <math.h>
// CYBOT 7

unsigned pulse_period = 320000; //  pulse period in cycles
unsigned mid_width = 304000; // Mid width compare in cycles
int angle = 90; // Stores the initial degree value for the servo motor
unsigned pulse_width = 0; // Stores the value of the pulse length
int direction = 1; // Stores the direction of the servo. 0 is left. 1 is right.

/// Initializes timer1
/** This method initializes timer 1 for pwm.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void timer1_init()
{

    //turn on clk for timer1
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;

    // TIMER CONFIGURATION
    //disable timer to config
    TIMER1_CTL_R &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

    //set size of timer to 16
    TIMER1_CFG_R = TIMER_CFG_16_BIT;

    //periodic and PWM enable
    TIMER1_TBMR_R |= (TIMER_TBMR_TBAMS | TIMER_TBMR_TBMR_PERIOD);
    TIMER1_TBMR_R &= ~TIMER_TBMR_TBCMR;

    // Need to set TBPWML field of GTPMCTL to inverted? Don't think so.

    // Set the maximum value of the counter
    TIMER1_TBILR_R = pulse_period & 0xFFFF;

    //lower 16 bits of the interval
    TIMER1_TBPR_R =  pulse_period >> 16;

    // Set the match values
    TIMER1_TBMATCHR_R = (pulse_period - 16000) & 0xFFFF;
    TIMER1_TBPMR_R = (pulse_period - 16000) >> 16;

    //enable timer
    TIMER1_CTL_R |= TIMER_CTL_TBEN;

}

/// Initializes the GPIO
/** This method initializes the gpio pins for pwm use.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void gpio_init() 
{

    //***set GPIO PB5, turn on clk, alt. function, output, enable***
    // GPIO CONFIGURATION
    //enable clock for port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Digital enable port B pin 5
    GPIO_PORTB_DEN_R |= 0b00100000;

    // set PB5 as output
    GPIO_PORTB_DIR_R |= 0x20;

    // Set output capture alternate function
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x00700000;

}

/// Move the servo to a certain degree measurement
/** This method moves the servo to a given dgree value.
 * @param degree The degree location to move the servo to.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void move_servo(int degree)
{

    // Calibrated width
    int calibrated_width = ((35866-8488)*(degree/180.0)) + 8488;

    pulse_width = calibrated_width;

    mid_width = pulse_period - calibrated_width;

    // Set the match values
    TIMER1_TBMATCHR_R = mid_width & 0xFFFF;
    TIMER1_TBPMR_R = mid_width >> 16;

    // Store the current angle value
    angle = degree;

    // Enorce delay for servo to move to position
    timer_waitMillis(50);

}
