/**
 * @file ping.c
 * @brief This file contains the source code for finding the distance from the ping sensor.
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/12/2018
 */

#include "Timer.h"
#include "lcd.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <math.h>

// Stores the last event time
volatile double last_event_time;
// Stores the states of the wave
volatile enum{LOW, HIGH, DONE} state;
// start time of the return pulse
volatile unsigned rising_time;
// end time of the return pulse
volatile unsigned falling_time;
// Stores the event time
volatile int event_time;
// Stores the edge of the wave
// 0 is positive edge, 1 is negative edge
volatile int edge = 0;
// Stores number of overflows
volatile int overflows = 0;
// Stores state of interrupt handle
volatile int interrupt_occurred = 0;

// Configures and initializes Timer3B
void timer3_init();
// Configures and initializes the PING))) sensor
void ping_init();
// Timer 3B ISR
void TIMER3B_Handler(void);
/* send out a pulse on PB3 */
void send_pulse();
/* start and read the ping sensor once, return distance in cm */
int ping_read();
/* convert time in clock counts to single-trip distance in cm */
double cycle2dist(int clock_cycles);

/// Timer 3B ISR
/** This method is the interrupt handler for timer3.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void TIMER3B_Handler(void)
{

    // clear the status flag
    TIMER3_ICR_R |= TIMER_ICR_CBECINT;

    // Determine whether there is a rising or falling edge
    if (edge == 0) { // && (TIMER3_MIS_R & 0x400) == 1
        rising_time = TIMER3_TBR_R;

		// Update edge status
        edge = 1;
        interrupt_occurred = 0;
    } else {
        falling_time = TIMER3_TBR_R;

		// Update edge and interrupt status
        edge = 0;
		interrupt_occurred = 1;

    }

}

/// Configures and initializes Timer3B
/** This method initializes the registers for timer 3 for use by the ping sensor.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void timer3_init() 
{

    // Turn on clock for timer3
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;

    // Disable Timer3B before changing settings
    TIMER3_CTL_R &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

    // clear the status flag
    TIMER3_ICR_R |= TIMER_ICR_CBECINT;

	// Split 32 bit timer into 2 16 bit timers
	TIMER3_CFG_R = TIMER_CFG_16_BIT;

	// Set to edge-time mode and set to capture mode
	TIMER3_TBMR_R |= TIMER_TBMR_TBMR_CAP;
	TIMER3_TBMR_R |= TIMER_TBMR_TBCMR;

	// Set timer to trigger on both edges
	TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH;

    // Load the timer start value
	TIMER3_TBPR_R = 0xFF;
	TIMER3_TBILR_R = 0xFFFF;

	// Set interrupt priority
	NVIC_PRI9_R |= 0x20;

	// Enable the NVIC
	NVIC_EN1_R |= 0x10;

	// Enable Timer3B
	TIMER3_CTL_R |= TIMER_CTL_TBEN;

    // Enable interrupts
    TIMER3_IMR_R |= TIMER_IMR_CBEIM;

	// Register the interrupt
    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    // Enable master interrupts
    IntMasterEnable();

}

/// Configures and initializes the PING))) sensor
/** This method configures the ping sensor.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void ping_init() 
{

    //enable ADC0 module on port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

	// Digital enable port B pin 4
	GPIO_PORTB_DEN_R |= 0b00001000;

    // set PB3 as input
    GPIO_PORTB_DIR_R &= 0xF7;

    // Set input capture alternate function
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R |= 0x00007000;

}

/// Sends out a pulse on PB3
/** This method sends out a pulse from the ping sensor.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void send_pulse()
{

    // Disable interrupts
    TIMER3_IMR_R &= ~TIMER_IMR_CBEIM;

    // clear the status flag
    TIMER3_ICR_R |= TIMER_ICR_CBECINT;

    // set PB3 as output
    GPIO_PORTB_DIR_R |= 0x08;

    // Remove the input capture function
    GPIO_PORTB_AFSEL_R &= 0xF7;

    // set PB3 to high
    GPIO_PORTB_DATA_R |= 0x08;

    // wait at least 5 microseconds based on data sheet
    timer_waitMicros(20);

    // set PB3 to low
    GPIO_PORTB_DATA_R &= 0xF7;

    // set PB3 as input
    GPIO_PORTB_DIR_R &= 0xF7;

    // Set input capture alternate function
    GPIO_PORTB_AFSEL_R |= 0x08;

    // clear the status flag
    TIMER3_ICR_R |= TIMER_ICR_CBECINT; // Remember this

	// Enable interrupts
	TIMER3_IMR_R |= TIMER_IMR_CBEIM;

}

/// Start and read the ping sensor once, return distance in cm
/** This method reads the distance value from the ping sensor.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
int ping_read()
{

	// Send a pulse
	send_pulse();

	while (interrupt_occurred == 0) {

	}

    // Find the width of the pulse
	event_time = (rising_time - falling_time);

//	// Check for overflow
	if (event_time < 0) {
		event_time = pow(2, 24) + event_time;
		overflows++;
	}

	// reset interrupt state
	interrupt_occurred = 0;

	// return pulse-width time in seconds
    return event_time;

}

/// convert clock_cycles in clock counts to single-trip distance in cm
/** This method converts the clock cycles returned from the ping sensor into a distance value.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
double cycle2dist(int clock_cycles) 
{

    return ((clock_cycles/16000000.0)/2.0) * 34000;

}
