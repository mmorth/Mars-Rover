# Mars-Rover
For the final project of my Embedded Systems class (CprE 288), I was a member of a four-student group that was tasked with navigating a Roomba through an obstacle course to a finishing zone. The obstacle course consisted of short, tall, and thick PVC pipes, missing floor tiles, and an out of bounds zone. The finishing zone was denoted by four thin, tall pieces of PVC pipe. The obstacle course simulated traversing a Mars Rover to a certain area to collect data without damaging it. A similar description of the project can be found here.

Our group approached this problem by breaking down all the requirements into steps that could be solved by a specific sensor on the Roomba. We started by using a Servo to sweep 180 degrees in front of the robot. While we were sweeping, we used an infrared sensor to determine the edges of the objects, which were used to calculate the width of the object. While an object was detected, we used an average value of the PING sensor to determine the distance the object is away from the robot. We then printed out each object’s distance away, degree locations, and front linear width. Then we used that information to manually draw out the course, so we could navigate to the finish zone. Finally, we could give inputs to the robot to tell it which way to move and for how much distance.

For this project, I mainly worked on the object detection code. I also created a human-readable output that allowed our group to easily determine where objects were on the course. Most of the knowledge required for the project was provided in lecture and the appropriate data sheets. We utilized TA help if we had further questions. This project helped me to learn how to analyze the values of many sensors to achieve a goal and how to break up a larger project into smaller parts that fit together to solve the larger problem. 

![alt text](https://github.com/mmorth/Mars-Rover/blob/master/Mars_Rover_Course.png)

# Running Code
The code for this project is specific to the platform we used for labs, so running this code is not possible without the use of that lab platform.
