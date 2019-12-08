#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_PWM2.h"                      // Header for this file

task_PWM2::task_PWM2 (const char* a_name,
						unsigned portBASE_TYPE a_priority,
						size_t a_stack_size,
						emstream* p_ser_dev
						)
	: frt_task(a_name, a_priority, a_stack_size, p_ser_dev)
{
	
	
}


void task_PWM2::run (void)
{
	
	delay_ms(10);
	portTickType previousTicks = xTaskGetTickCount ();

	for(;;)
	{
		switch (state)
		{
			case(INIT):
			counter = 0;
			PORTE.DIR = 0b00100010;			// Set pin 1 and 5 direction to output
			PORTE.OUTSET = 0b00100010;		// Set pin 1 and 5 port E to high
			PORTE.DIRSET = 0b00100010;		// Set pin 1 and 5 port E to out
			TCE0.CTRLB = 0b00100011;		// enable capture for CCB and Waveform to single slope
			TCE0.PER = 10000;				// Set the period tick 
			TCE0.CTRLA= 0b00000101;			// Clock Scaler CLK/64
			TCE0.CCB = 1000;				// Set the compare tick
			transition_to(GO);				// Transition to GO state
			//TCE1.CCB = 1280;
			break;
			
			case(GO):
			uint16_t storage1 = xpointer->get();		// Get data from x pointer queue
			TCE0.CCB = storage1*5;					    // Set the compare tick value from shared datat with gain of 5
			break;
			

		};
		runs++;
		delay_ms(200);
	}
}