/*
 * detect.h
 *
 *  Created on: Mar 13, 2018
 *      Author: mmorth
 */

#ifndef DETECT_H_
#define DETECT_H_

// Moves the servo from 0-180 degrees and returns degree, IR, and ping sensor values
void sweep_measure();

// Moves the servo from 0-180 degrees and returns degree, IR, and ping sensor values
void sweep_measure_record();

#endif /* DETECT_H_ */
