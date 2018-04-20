/*
 * Header file for UI.c file
*/

#ifndef UI_H_
#define UI_H_

// Sends commands to the robot
void robot_command();

// Signals the robot is in the finishing location by sounding the alarm and flashing the LED.
void finish();

// Sweeps for objects. Returns:
// Object distances away from robot, linear widths, degree width, and object number
void sweep_info();

#endif /* UI_H_ */
