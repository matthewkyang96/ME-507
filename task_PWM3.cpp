#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "shared_data_sender.h"
#include "shared_data_receiver.h"
#include "task_PWM3.h"                      // Header for this file

task_PWM3::task_PWM3 (const char* a_name,
						unsigned portBASE_TYPE a_priority,
						size_t a_stack_size,
						emstream* p_ser_dev
						)
	: frt_task(a_name, a_priority, a_stack_size, p_ser_dev)
{
	
	
}


void task_PWM3::run (void)
{
	
	delay_ms(10);
	portTickType previousTicks = xTaskGetTickCount ();

	for(;;)
	{
		switch (state)
		{
			case(INIT):
			counter = 0;
			PORTD.DIR = PIN5_bm;		// Set 5 direction to output
			PORTD.OUTSET = PIN5_bm;     // Set 5 port D to high
			PORTD.DIRSET = PIN5_bm;     // Set 5 port D to out
			TCD1.CTRLB = 0b00100011;	// enable capture for CCB and Waveform to single slope
			TCD1.PER = 10000;			// Set the period tick
			TCD1.CTRLA=0b00000101;		// Clock Scaler CLK/64
			TCD1.CCB = 1000;			// Set the compare tick
			transition_to(GO);			// transition to go state
			break;
			
			case(GO):
			uint16_t storage2 = ypointer->get();		// Get data from y pointer queue
			TCD1.CCB = storage2*50;						// Set the compare tick value from storage with gain of 50
			//transition_to(INIT);
			break;
			

		};
		runs++;
		delay_ms(200);
	}
}