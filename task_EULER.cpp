//**************************************************************************************
/** \file EULER_LED.cpp
 *    This file contains source code for a user interface task for a ME405/FreeRTOS
 *    test suite. 
 *
 *  Revisions:
 *    \li 09-16-2018 CTR Adapted from JRR task_user.cpp
 *    \li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    \li 10-05-2012 JRR Split into multiple files, one for each task
 *    \li 10-25-2012 JRR Changed to a more fully C++ version with class task_user
 *    \li 11-04-2012 JRR Modified from the data acquisition example to the test suite
 *
 *  License:
 *    This file is copyright 2012 by JR Ridgely and released under the Lesser GNU 
 *    Public License, version 2. It intended for educational use only, but its use
 *    is not limited thereto. */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 *    TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 *    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 *    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//**************************************************************************************

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_EULER.h"                      // Header for this file
#include "twim.h"


//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */

task_EULER::task_EULER (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size,
					  emstream* p_ser_dev
					 )
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/** This task calls values from the MMA8451 Accelerometer
 */

void task_EULER::run (void)
{
	// Make a variable which will hold times to use for precise task scheduling
	portTickType previousTicks = xTaskGetTickCount ();

	// Wait a little while for user interface task to finish up
	delay_ms(10);
	

    //brief Input parameters when initializing the twi module mode

	
	twi_options_t options;			
	options.chip = 0x50;			//! The desired address.
	options.speed = 100000;			//! The baudrate of the TWI bus.
	options.speed_reg = 255;		//! The baudrate register value of the TWI bus.
	
	// Initialize the master module with the options stated above		
	twi_master_enable(&TWIC);                
	twi_master_init(&TWIC,&options);
			 
	// Array to store data for initialization
	uint8_t initdata[1] = {0x03};		 
	// brief Information concerning the data transmission for initialization.		 
	twi_package_t packageinit;				 
	packageinit.addr[0] = 0x2A;		// TWI address/commands to issue to the other chip (node).
	packageinit.addr_length = 1;	// Length of the TWI data address segment (1-3 bytes).
	packageinit.buffer = initdata;  // Where to find the data to be written.
	packageinit.chip = 0b0011101;   // TWI chip address to communicate with.
	packageinit.length = 1;			// How many bytes do we want to write.
	packageinit.no_wait = false;	// Whether to wait if bus is busy (false) or return immediately (true)
	
	// Array to store data for eulerxydata		
	uint8_t eulerxydata[3] = {0,0,0};
	// brief Information concerning the data transmission for package xy.
	twi_package_t eulerpackagexy;   
	eulerpackagexy.addr [0] = 0x01;		 // TWI address/commands to issue to the other chip (node).
	eulerpackagexy.addr [1] = 0x03;		 // TWI address/commands to issue to the other chip (node).
	eulerpackagexy.addr_length = 2;		 // Length of the TWI data address segment (1-3 bytes).
	eulerpackagexy.buffer = eulerxydata; // Where to find the data to be written.
	eulerpackagexy.chip = 0b0011101;     // TWI chip address to communicate with.
	eulerpackagexy.length = 2;			 // How many bytes do we want to write.
	eulerpackagexy.no_wait = false;      // Whether to wait if bus is busy (false) or return immediately (true)
	
	// Array to store data for eulerzdata		
	uint8_t eulerzdata[1] = {0};
	// brief Information concerning the data transmission for package z.
	twi_package_t eulerpackagez;
	eulerpackagez.addr [0] = 0x05;			// TWI address/commands to issue to the other chip (node).
	eulerpackagez.addr_length = 1;			// Length of the TWI data address segment (1-3 bytes).
	eulerpackagez.buffer = eulerzdata;		// Where to find the data to be written.
	eulerpackagez.chip = 0b0011101;			// TWI chip address to communicate with.
	eulerpackagez.length = 1;				// How many bytes do we want to write.
	eulerpackagez.no_wait = false;			// Whether to wait if bus is busy (false) or return immediately (true)
	
	//set SDA/SCL pins for the master TWI to be wired and enable pull-up.		
	PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	
	// write our packet through the master TWI module
	twi_master_write(&TWIC,&packageinit);
	
	// main loop to run continuously to grab xyz data
	while(1)
	{
		switch (state)
		{
		case (start):
			twi_master_read(&TWIC,&eulerpackagexy);  // read from master TWI module 
			twi_master_read(&TWIC,&eulerpackagez);	 // read from master TWI module
			int x = eulerxydata[0];					 // Variable x for euler data x
			int y = eulerxydata[1];					 // Variable y for euler data y
			int z = eulerzdata[0];					 // Variable z for euler data z
			xpointer->put(x);						 // Put x data into xpointer queue
			ypointer->put(y);						 // Put y data into xpointer queue
			zpointer->put(z);						 // Put z data into xpointer queue
			break;
			
		
		};
		runs++;										// Increment Runs
		delay_from_to_ms(previousTicks,10);			// Delay
	}
}