/*
 * ping.h
 *
 *  Created on: Mar 3, 2018
 *      Author: mmorth
 */

#ifndef PING_H_
#define PING_H_

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

#endif /* PING_H_ */
