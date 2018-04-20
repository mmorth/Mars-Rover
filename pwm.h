/*
 * pwm.h
 *
 *  Created on: Mar 13, 2018
 *      Author: mmorth
 */

#ifndef PWM_H_
#define PWM_H_

// Initialize the timer
void timer1_init();

// Initializes the GPIO
void gpio_init();

// Move the servo to a certain degree measurement
void move_servo(unsigned degree);

// Complete a certain servo task based on which button was pressed
void button_execution(uint8_t button_value);

#endif /* PWM_H_ */
