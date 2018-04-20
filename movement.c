/**
 * @file movement.c
 * @brief This file contains the source code for the movement of the robot.
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/12/2018
 */

// Include necessary files
#include "movement.h"
#include "open_interface.h"
#include "lcd.h"
#include"timer.h"
#include"uart.h"
#include <string.h>

/// Moves the robot forward a given amount
/** This method moves the robot forward a given amount without using any sensors to detect objects.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot forward.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void move_forward(oi_t *sensor, int millimeters) 
{

    // Have the robot move 50 millimeters forward then stop
    int sum = 0;
    oi_setWheels(125, 100); // move forward; full speed

    // Move robot forward
    while (sum < millimeters) {
        oi_update(sensor);
        sum += sensor->distance;
    }

    // Have the robot stop once it has traveled 1 meter
    oi_setWheels(0, 0); 

}

/// Turns the robot a given amount
/** This method turns the robot a specified amount left or right.
 * @param sensor The Roomba sensor information.
 * @param degrees The number of degrees to turn the robot. A positive degree is a right turn. A negative degree is a left turn.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void turn(oi_t *sensor, int degrees) 
{

    // Have the robot turn the specified degrees
    int sum = 0;
    if (degrees > 0) {
        oi_setWheels(250, -250);
    } else {
        oi_setWheels(-250, 250);
    }
    if (degrees > 0) {
        while (sum < degrees) {
            oi_update(sensor);
            sum += sensor->angle;
        }
    } else {
        while (sum > degrees - 5) {
            oi_update(sensor);
            sum += sensor->angle;
        }
    }

    // Have the robot stop once it has turned the specified degree value
    oi_setWheels(0, 0);

}

/// Moves to avoid the obstacle
/** This method is called to avoid objects. The robot will backup, turn, then face forward again.
 * @param sensor The Roomba sensor information.
 * @param direction The direction the object was detected.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void avoid_obstacle(oi_t *sensor, int direction) 
{

    // Move the robot to avoid the obstacle
    move_backward(sensor, -150);
    if (direction == 0) {
        turn(sensor, 90);
        move_forward(sensor, 250);
        turn(sensor, -90);
    } else {
        turn(sensor, -90);
        move_forward(sensor, 250);
        turn(sensor, 90);
    }

}

/// Moves the robot backward a specified amount
/** This method moves the robot backward a specified distance in millimeters.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot backward.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void move_backward(oi_t *sensor, int millimeters) 
{
    // Have the robot millimeters backward then stop
    int sum = 0;
    oi_setWheels(-110, -100); // move forward; full speed

    // Move robot backward
    while (sum > millimeters) {
        oi_update(sensor);
        sum += sensor->distance;
    }

    // Have the robot stop once it has traveled 1 meter
    oi_setWheels(0, 0); // stopoi_free(sensor_data); }
}

/// Moves the robot forward a given amount
/** This method moves the robot forward and returns the distance the robot actually moved forward in case it bumped into an object.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot forward.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
int move_forward_return(oi_t *sensor, int millimeters) 
{

    // Have the robot move 50 millimeters forward then stop
    int sum = 0;
    int bumperHit = 0;
    oi_setWheels(250, 250); // move forward; full speed

    // Move robot forward
    while (sum <= millimeters) {
        oi_update(sensor);
        sum += sensor->distance;
        if (sensor->bumpLeft == 1) {
            avoid_obstacle(sensor, 1);
            bumperHit = 1;
            break;
        } else if (sensor->bumpRight == 1) {
            avoid_obstacle(sensor, 0);
            bumperHit = 1;
            break;
        }
    }

    // Have the robot stop
    oi_setWheels(0, 0); // stopoi_free(sensor_data); }

    // Update distance travelled if object was hit
    if (bumperHit == 1) {
        sum -= 150;
        bumperHit = 0;
    }

    return sum;
}

/// Moves the robot forward a given amount and detect the cliff
/** This method moves the robot forward and returns the distance the robot actually moved forward in case it detected a cliff.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot forward.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void move_forward_cliff(oi_t *sensor, int millimeters) 
{

    int sum = 0;
    oi_setWheels(20, 20); // move forward

    // Move robot forward
    while (sum < millimeters) {
        oi_update(sensor);
        if(sensor->cliffFrontLeft ||sensor->cliffLeft || sensor->cliffFrontRight ||sensor->cliffRight)
        {
            move_backward(sensor, -100);
            break;
        }
        sum += sensor->distance;
    }

    // Stop robot
    oi_setWheels(0, 0);

}

/// Moves the robot forward a given amount and detect the white tape
/// Moves the robot forward a given amount
/** This method moves the robot forward and returns the distance the robot actually moved forward in case it detects the white tape.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot forward.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void move_forward_color(oi_t *sensor, int millimeters) 
{

    int sum = 0;
    oi_setWheels(20, 20); // move forward

    // Move forward
    while (sum < millimeters) {
        oi_update(sensor);
        lcd_printf("%d",sensor->cliffFrontLeftSignal);
        if(sensor->cliffFrontLeftSignal>2600 ||sensor->cliffLeftSignal>2600  || sensor->cliffFrontRightSignal>2600  ||sensor->cliffRightSignal>2600)
        {
            move_backward(sensor, -100);
            break;
        }
        sum += sensor->distance;
    }

    // Stop robot
    oi_setWheels(0, 0);

}

/// Moves the robot forward a given distance until an object is detected.
/** This method moves the robot forward and returns the distance the robot actually moved forward in case it bumped into an object, detects a cliff, or detects the white tape.
 * @param sensor The Roomba sensor information.
 * @param millimeters The number of millimeters to move the robot forward.
 * @return The number of millimeters the robot actuall traveled until it stopped.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
int move_forward_amount(oi_t *sensor, int millimeters) 
{

    int sum = 0;
    oi_setWheels(110, 100); // move forward
//        uart_sendStr("Cliff: Left FrontLeft FrontRight Right\n\r");
    uart_sendStr("LBump LCliff LCliffS FLCliff FLCliffS FRCliff FRCliffS RCliff RCliffS RBump \n\r");
    // Move forward
    while (sum < millimeters) {
        oi_update(sensor);
           char message[100];
           sprintf(message, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n\r",
                   sensor->bumpLeft, sensor->cliffLeft,
                   sensor->cliffLeftSignal, sensor->cliffFrontLeft,
                   sensor->cliffFrontLeftSignal, sensor->cliffFrontRight,
                   sensor->cliffFrontRightSignal, sensor->cliffRight,
                   sensor->cliffRightSignal, sensor->bumpRight);
           uart_sendStr(message);

        // Detect sensors
        if(sensor->cliffFrontLeftSignal>2700 ||sensor->cliffLeftSignal>2700  || sensor->cliffFrontRightSignal>2700  ||sensor->cliffRightSignal>2700||sensor->cliffFrontLeft ||sensor->cliffLeft || sensor->cliffFrontRight ||sensor->cliffRight||sensor->bumpLeft||sensor->bumpRight)
        {
            move_backward(sensor, -100);
            return sum-100;
        }
        sum += sensor->distance;
    }

    // Stop the robot
    oi_setWheels(0, 0);
    return sum;

}
