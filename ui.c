/**
 * @file ui.c
 * @brief This file contains the source code for the user interface
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/14/2018
 */

// Include all necessary files
#include "movement.h"
#include "detect.h"
#include "Timer.h"
#include "lcd.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <math.h>
#include "distance.h"
#include "ping.h"
#include "pwm.h"
#include "open_interface.h"
#include "ui.h"
#include <string.h>
#include "uart.h"

// The sensor data variable
oi_t *sensor_data;
int amount = 0;

// Define a constant for PI
#define M_PI 3.14159265358979323846

///// Accepts input for moving and sweeping the robot.
///**
// * This method is used to accept inputs for moving, turning, stopping sweeping, and sending the finish command to the robot.
// * This method also sends back information about the status of the sensors.
// * The following information below explains which button to press for each command.
// * p = sweep
// * c = send finish command
// * f = move forward
// * b = move backward
// * t = turn
// * s = stop
// * @param sensor The Roomba sensor information.
// * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
// * @date 4/12/2018
// */
void robot_command()
{

    // Signal to mission control that a new command has been sent.
    uart_sendStr("New Command: \n\r");

    // Receive the command
    char command = uart_receive();

    if (command == 'p')
    { // get sweep information
        sweep_info();
        uart_sendStr("Sweep Done.\n\r");
    }
    else if (command == 'c')
    { // robot is in finishing position
        finish();
        uart_sendStr("Retrieval Complete.\n\r");
    }

    else if (command == 'f')
    { // move robot forward
        uart_sendStr("Enter amount to move forward:");
        amount = (uart_receive() - '0') * 100;
        uart_sendStr("\n\r");

        // Correct amount if a larger value was accidentally entered
        if (amount > 400)
        {
            amount = 400;
        }

        int dist_moved = move_forward_amount(sensor_data, amount);
        uart_sendStr("\n\rDistance moved: ");
        char dist[20];
        sprintf(dist, "%d", dist_moved);
        uart_sendStr(dist);
        uart_sendStr("\n\r");
    }

    else if (command == 'l')
    { // turn left
        uart_sendStr("Enter amount to turn left:");
        amount = (uart_receive() - '0') * 10;

        // Correct amount if a larger value was accidentally entered
        if (amount > 90 || amount < 10)
        {
            uart_sendStr("\n\rInvalid input.\n\r");

        }
        else{
        uart_sendStr("\n\r");
        turn(sensor_data, amount);
        uart_sendStr("Turn Complete.\n\r");
        }
    }

    else if (command == 'r')
    { // turn right
        uart_sendStr("Enter amount to turn right:");
        amount = (uart_receive() - '0') * -10;

        // Correct amount if a larger value was accidentally entered
        if (amount < -90 || amount > -10)
        {
            uart_sendStr("\n\rInvalid input.\n\r");
        }
        else{
        uart_sendStr("\n\r");
        turn(sensor_data, amount);
        uart_sendStr("Turn Complete.\n\r");
        }
    }

    else if (command == 's')
    { // stop robot
        oi_setWheels(0, 0);
        uart_sendStr("Retrieval Complete.\n\r");
    }

}

///// Flashes the power light and plays song for robot.
///** This method signals that the robot is in the finishing location by playing a "We Are the Champions" by Queen and flashing the power LED.
// * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
// * @date 4/8/2018
// */
void finish()
{
    // Turn on the LED
    oi_setLeds(0, 0, 0, 255);
    timer_waitMillis(250);
    oi_setLeds(0, 0, 0, 0);
    timer_waitMillis(250);
    oi_setLeds(0, 0, 0, 255);
    timer_waitMillis(250);
    oi_setLeds(0, 0, 0, 0);

    unsigned char notes[41] = { 62, 61, 62, 61, 57, 54, 59, 54, 57, 62, 64, 66,
                                69, 66, 59, 61, 59, 59, 57, 59, 57, 55, 67, 66,
                                67, 66, 64, 66, 62, 67, 66, 62, 67, 65, 62, 67,
                                65, 62, 60, 57, 62 };

    unsigned char duration[41] = { 32, 16, 16, 32, 32, 32, 32, 16, 32, 16, 16,
                                   32, 32, 16, 16, 16, 32, 32, 32, 16, 16, 32,
                                   32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
                                   32, 32, 32, 32, 32, 16, 16, 64 };

    // Load the song
    oi_loadSong(1, 41, notes, duration);

    // Play the song
    oi_play_song(1);
}

///// Sweep for tall objects.
///**
// * This method is used to make the servo, ping, and ir sensors sweep and detect tall objects that are 180 degrees in front of the robot.
// * It sends to Putty the detected object information as well as the degree location of the servo,
// * the distance reading on the ir sensor, and the distance reading on the ping sensor.
// * The sweep distance is between 10-50cm in front of the robot.
// * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
// * @date 4/8/2018
// */
void sweep_info()
{
    // Move the servo to 0 degrees and wait until the servo moves to that position.
    move_servo(0);

    // Stores the degree measurement
    char degree = 0;

    // Set objects array element to 0.
    char element = 0;

    // Sets the object number equal to 0.
    char object_num = 1;

    // Stores the starting degree position of the detected object.
    char start_degree = 0;

    // Stores the number of degrees the current object has been detected for.
    int detected_degrees = 0;

    /** Stores the information for each object detected */
    float objects[100];

    // Stores the average ping distance
    float average_ping = 0;

    float ping_sum = 0;

    // Sweep for each degree between 0 and 180 degrees
    for (degree = 0; degree <= 180; degree++)
    {
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
        if (ir_distance <= 50 && ping_distance <= 50 && ir_distance >= 10
                && degree != 180)
        {

            // Store the starting degree of the object
            if (detected_degrees == 0)
            {
                start_degree = degree;
            }

            // increment number of degrees the object has been detected
            detected_degrees++;
            ping_sum = ping_sum + ping_distance;

        }
        else
        {
            // Check for faulty data
            if (detected_degrees >= 5)
            { // valid data. Process

                average_ping = ping_sum / detected_degrees;

                // Determine front linear width of object
                double object_width = average_ping * 2
                        * tan((detected_degrees / 2.0) * (M_PI / 180));

                // initialize and set the for loop variable
                int i = 0;

                // Store the object information
                for (i = 0; i < 4; i++)
                {
                    if (i == 0)
                    { // store the object number
                        objects[element] = (double) object_num;
                    }
                    else if (i == 1)
                    { // store the distance the object is away from the sensor
                        objects[element] = (double) average_ping;
                    }
                    else if (i == 2)
                    { // store the linear width of the object
                        objects[element] = (double) object_width;
                    }
                    else
                    { // store the angular range
                        objects[element] = (double) start_degree;
                        element++;
                        objects[element] = (double) degree - 1; // ending degree
                    }
                    element++;
                }

                // increase object number
                object_num++;

                // Reset number of degrees object has been detected to zero
                detected_degrees = 0;

                average_ping = 0;
                ping_sum = 0;

            }
            else
            { // faulty data. Do not process
              // Reset number of degrees object has been detected to zero
                detected_degrees = 0;
                average_ping = 0;
                ping_sum = 0;
            }
        }

        // Send data to Putty
        char message[100];
        sprintf(message, "%d\t%.2lf\t\t%.2lf\n\r", degree, ir_distance,
                ping_distance);
        uart_sendStr(message);

    }

    // Move the servo to 0 degrees and wait until the servo moves to that position.
    move_servo(0);

    // Send the object information back to Putty
    char object_message[100];

    int i = 0;

    for (i = 0; i < element; i = i + 5)
    {
        sprintf(object_message,
                "\nNEW OBJECT:\n\rObject: %.2lf\n\rAvg_Ping: %.2lf\n\rWidth: %.2lf\n\rStart: %.2lf\n\rEnd: %.2lf\n\r",
                objects[i], objects[i + 1], objects[i + 2], objects[i + 3],
                objects[i + 4]);
        uart_sendStr(object_message);
    }

}

///// Main method
///** Main method for Mars Rover project. This calls and runs all the information.
// * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
// * @date 4/8/2018
// */
void main()
{
    // Initialize the button and lcd
    lcd_init();

    // Initialize the uart
    uart_init();

    //Initialize the open interface
    sensor_data = oi_alloc();
    oi_init(sensor_data);

    // Initialize the IR sensor
    adc_init();

    // Initialize the Ping sensor
    ping_init();
    timer3_init();

    // Initialize the servo
    timer1_init();
    gpio_init();

    // Move servo to the initial position of 0 degrees
    move_servo(0);

    // Loop forever
    while (1)
    {
        // Receive the signal on how to move the robot
        robot_command();
    }
}
