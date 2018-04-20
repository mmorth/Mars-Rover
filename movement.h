/*
 * movement.h
 *
 *  Created on: Jan 22, 2018
 *      Author: mmorth
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"

// Moves the robot forward a specified amount
void move_forward(oi_t *sensor, int centimeters);

// Moves the robot forward until it hits an object, then it returns the distance traveled
int move_forward_return(oi_t *sensor, int centimeters);

// Turns the robot a specified amount
void turn(oi_t *sensor, int degrees);

// Moves the robot backward a specified amount
void move_backward(oi_t *sensor, int centimeters);

// Move the robot forward a specified amount
int move_forward_amount(oi_t *sensor, int millimeters);

#endif /* MOVEMENT_H_ */
