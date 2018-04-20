# Mars-Rover
For the final project of my Embedded Systems class (CprE 288), I was a member of a four member group that was tasked with navigating a Roomba through a obstacle course to a finishing zone. The obstacle course consisted of short and tall, thick PVC pipes, missing floor tiles, and an out of bounds zone. The finishing zone was denoted by four thin, tall pieces of PVC pipes. The obstacle course simulated traversing a mars rover to a certain area to collect data without damaging the rover.

Our group approached this problem by using a Servo to sweep 180 degrees in front of the robot. While we were sweeping, we used an infrared sensor to determine the edges of the objects. While an object was detected, we used an average value of the PING sensor to determine the distance the object is away from the robot. We then printed out each object’s distance away, degree locations, and front linear width. We then used that information to manually draw out the course, so we could navigate to the finish zone. For this project, I mainly worked on creating the Servo sweeping code. I also created a human-readable output that allowed our group to easily determine where objects were on the course.

# Running Code
The code for this project is specific to the platform we used for labs, so running this code is not possible without the use of that lab platform.
