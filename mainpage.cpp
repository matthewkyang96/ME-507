/*! \mainpage Project 5 DOF
 *
 *
 *
 * \section Purpose 
 * The purpose of this project is to create a 5 Degree of Freedom robot that can be driven using an orientation sensor.
 * The robot will resemble the movement of a wrist, shoulder and elbow joint.  The user will be able to move the robot as
 * the person moves the sensor.
 *
 * \section Usage
 * The appropriate pins will need to be set up to have the system running properly.
 * The pin description are outlined in the classes in the other subsections. The I2C data
 * bus must be enabled and able to read the digital readings form the BNO055_Base driver.
 * THe following code can be directly copied and pasted into the ATXEMG128a3u micro controller to
 * run the robot.  A custom made PCB board was created to contain the micro controller, USART interface,
 * and motor drivers.  A power source must be provided to run the PCB board.  Once the code is initialized
 * control of the robot can begin.
 *
 * \section Testing
 * Due to unforeseen circumstances, the project scope was changed to actuate 3 degrees of freedom or the wrist motion.
 * The custom PCB board was built progressively ensuring that all connections were receiving correct voltages.  Once the
 * board was configured properly, PWM code was tested to actuate a servo motor.  Separate tasks were configured to actuate 
 * each PWM servo motor.  An oscilloscope was used to read the frequency output from each signal and a gain was applied
 * to actuate the signal from a low of 1.5 ms to a high of 2.5 ms. The sensor was then enabled and was able to share data
 * with each PWM signal to actuate the motors to a given position.
 *
 * \section Bugs
 * A 3-axis accelerometer was used instead of the BNO055 due to problems reading from the sensor.
 *
 * \section Limitations
 * Due to using a new sensor, the data transmitted was acceleration data and not Euler angles data.  The acceleration data 
 * was not conducive because position should be controlled with a given angle not acceleration due to movement of the sensor.
 * Also the 3-axis sensor would only to give tilt angles in two directions due to its refrencing of gravity.
 * 
 *
 * \section Location
 *
 */
  