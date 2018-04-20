/**
 * @file detect.c
 * @brief This file contains the source code for detecting objects.
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
#include "distance.h"
#include "ping.h"
#include "pwm.h"
#include "uart.h"

#define M_PI 3.14159265358979323846

int index = 0; // Stores the index of the smallest object
double distance_away = 0; // Stores the distance the object is away from the sensor in cm
double distance_width = 0; // Stores the width of the front of the object in cm
int degrees_object_detected = 0; // Stores the number of degrees an object was detect
int smallest_object = 0; // Stores the number of degrees an object was detected for
int smallest_index = 0; // Stores the index of the smallest object found
int degree_location = 0; // Stores the degree location of the smallest object.
int previous_distance = 0; // Stores the previous sensed distance.

/// Moves the servo from 0-180 degrees and returns degree, IR, and ping sensor values
/** This method sweeps for objects that are 180 degree in front of the robot. It returns the servo degree and ir and ping distances.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void sweep_measure() 
{

    // Stores the degree measurement
    int degree = 0;

	// Sweep for each degree between 0 and 180 degrees
	for (degree = 0; degree <= 180; degree++) {
		// Move servo to degree position
		move_servo(degree);
		
		// Determine IR sensor values
		adc_receive();
		int quantization = ADC_read(0);
		double ir_distance = convert_distance(quantization);
		
		// Determine Ping sensor values
		int time = ping_read();
        double ping_distance = cycle2dist(time);
		
		// Send data to Putty
        char message[100];
        sprintf(message, "%d\t%.2lf\t\t%.2lf\n\r", degree, ir_distance, ping_distance);
		uart_sendStr(message);
		
	}

}

/// Moves the servo from 0-180 degrees and returns degree, IR, and ping sensor values and moves the servo to the smallest object detected.
/** This method sweeps for objects that are 180 degree in front of the robot. It returns the servo degree and ir and ping distances.
 * It also keeps track and moves the servo in the direction of the smallest object.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void sweep_measure_record() 
{

    // Stores the degree measurement
    int degree = 0;

    // Sweep for each degree between 0 and 180 degrees
	for (degree = 0; degree <= 180; degree++) {
		// Move servo to degree position
		move_servo(degree);
		
		// Determine IR sensor values
		adc_receive();
		int quantization = ADC_read(0);
		double ir_distance = convert_distance(quantization);
		
		// Determine Ping sensor values
		double time = ping_read();
        double ping_distance = cycle2dist(time);
		
		// Determine degree width of object
		if (ir_distance <= 80 && degree != 180) { // && abs(ir_distance - previous_distance) < 10
			// increment number of degrees the object has been detected
			degrees_object_detected++;
			// previous_distance = ir_distance;
		} else {
			// previous_distance = ir_distance;
			// Check for faulty data
			if (degrees_object_detected > 1) { // valid data. Process
			
				// Determine width of object
				double object_width = ping_distance * 2 * tan((degrees_object_detected/2.0) * (M_PI/180));
				
				printf("%d\t%d\n", degrees_object_detected, degree);

				// Increase the number of object detected
				index++;
				
				// Determine if object is smaller than the smallest one found so far
				if (object_width < distance_width) { // compare against smallest value
					smallest_object = degrees_object_detected;
					smallest_index = index;
					distance_away = ping_distance;
					degree_location = degree - degrees_object_detected/2;
					distance_width = object_width;
				} else { // if first object detected
					if (distance_width == 0) {
						smallest_object = degrees_object_detected;
						smallest_index = index;
						distance_away = ping_distance;
						degree_location = degree - degrees_object_detected/2;
						distance_width = object_width;
					}
				}

				// Reset number of degrees object has been detected to zero
				degrees_object_detected = 0;
			} else { // faulty data. Do not process
				degrees_object_detected = 0;
			}
		}
		
        // Send data to Putty
        char message[100];
        sprintf(message, "%d\t%.2lf\t\t%.2lf\n\r", degree, ir_distance, ping_distance);
        uart_sendStr(message);
		
	}

}
