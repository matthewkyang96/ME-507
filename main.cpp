//*************************************************************************************
/** \file lab1_main.cpp
 *    This file contains the main() code for a program which runs a port of the FreeRTOS
 *    for AVR devices. This port is specific to the XMEGA family.
 *
 *  Revisions:
 *    \li 09-14-2017 CTR Adapted from JRR code for AVR to be compatible with xmega 
 *
 *  License:
 *    This file is released under the Lesser GNU Public License, version 2. This 
 *    program is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************


#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <avr/interrupt.h>
#include <string.h>                         // Functions for C string handling

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues
#include "croutine.h"                       // Header for co-routines and such

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "frt_task.h"                       // Header of wrapper for FreeRTOS tasks
#include "frt_text_queue.h"                 // Wrapper for FreeRTOS character queues
#include "frt_queue.h"                      // Header of wrapper for FreeRTOS queues
#include "frt_shared_data.h"                // Header for thread-safe shared data
#include "shares.h"                         // Global ('extern') queue declarations

#include "xmega_util.h"

#include "task_user.h"                     // Header for user interface task
#include "task_PWM1.h"					   // Header for Actuating Vertical Movement servo
#include "task_PWM2.h"					   // Header for actuating horizontal movement servo
#include "task_PWM3.h"					   // Header for actuating twist moevment servo
#include "task_EULER.h"					  // Header for euler/acceleration data

#include "twim.h"						  // Header for i2c communication
#include "shares.h"						  // Header for sharing variables between tasks

frt_text_queue print_ser_queue (32, NULL, 10);  

shared_data<uint16_t>* xpointer;		 // X variable data pointer
shared_data<uint16_t>* ypointer;         // y variable data pointer
shared_data<uint16_t>* zpointer;         // z variable data pointer



//=====================================================================================
/** The main function sets up the RTOS.  Some test tasks are created. Then the 
 *  scheduler is started up; the scheduler runs until power is turned off or there's a 
 *  reset.
 *  @return This is a real-time microcontroller program which doesn't return. Ever.
 */

int main (void)
{
		
	cli();
	// Configure the system clock to use internal oscillator at 32 MHz
	config_SYSCLOCK();
	
	// Disable the watchdog timer unless it's needed later. This is important because
	// sometimes the watchdog timer may have been left on...and it tends to stay on	 
	wdt_disable ();


	// Configure a serial port which can be used by a task to print debugging infor-
	// mation, or to allow user interaction, or for whatever use is appropriate.  The
	// serial port will be used by the user interface task after setup is complete and
	// the task scheduler has been started by the function vTaskStartScheduler()
	rs232 ser_dev(0,&USARTF0); // Create a serial device on USART E0
	ser_dev << clrscr << "FreeRTOS Xmega Testing Program" << endl << endl;
	
	// user interface task
	new task_user ("UserInt", task_priority (0), 260, &ser_dev);
	
	// The PWM task is at high priority and is used to send a PWM signal to each 
	// servo to move to a certain position based on shared data from the
	// Euler task
	
	new task_PWM1 ("vertical_movmement", task_priority (2), 260, &ser_dev);
	new task_PWM2 ("horizontal_movement", task_priority (2), 260, &ser_dev); 
	new task_PWM3 ("twist_movement", task_priority (2), 260, &ser_dev);
	
	// The Euler task is run at low priority and is used to get acceleration data from the 
	// *_______* sensor
	new task_EULER ("angle", task_priority (1), 260, &ser_dev);
	
	// Create pointers for each shared_data axial information
	xpointer = new shared_data<uint16_t>;         
	ypointer = new shared_data<uint16_t>;
	zpointer = new shared_data<uint16_t>;
	
	// Enable high - low level interrupts and enable global interrupts
	PMIC_CTRL = (1 << PMIC_HILVLEN_bp | 1 << PMIC_MEDLVLEN_bp | 1 << PMIC_LOLVLEN_bp);
	sei();

	// Here's where the RTOS scheduler is started up. It should never exit as long as
	// power is on and the microcontroller isn't rebooted
	vTaskStartScheduler ();
}