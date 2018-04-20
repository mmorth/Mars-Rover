/**
 * @file distance.c
 * @brief This file contains the source code for using the ir distance.
 *
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 *
 * @date 4/12/2018
 */

#include "lcd.h"
#include "Timer.h"
#include <math.h>

/// Method that initializes the ADC
/** This methods initializes the registers required for ACD0 and SS1.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void adc_init(void) 
{

	//enableADC 0 module on port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	
	//enable clock for ADC
	SYSCTL_RCGCADC_R |= 0x1;
	
	//enable port B pin 0 to work as alternate functions
	GPIO_PORTB_AFSEL_R |= 0x04;
	
	//set pin to input -0
	GPIO_PORTB_DEN_R &= 0b11111011;
	
	//disable analog isolation for the pin
	GPIO_PORTB_AMSEL_R |= 0x04;
	
	//initialize the port trigger source as processor (default)
	GPIO_PORTB_ADCCTL_R = 0x00;
	
	//disable SS0 sample sequencer to configure it
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN1;
	
	//initialize the ADC trigger source as processor (default)
	ADC0_EMUX_R = ADC_EMUX_EM1_PROCESSOR;
	
	//set 1st sample to use the AIN10 ADC pin
	ADC0_SSMUX1_R |= 0x000A;
	
	//enable raw interrupt status
	ADC0_SSCTL1_R |= (ADC_SSCTL1_IE0 | ADC_SSCTL1_END0);
	
	//enable oversampling to average
	ADC0_SAC_R |= ADC_SAC_AVG_64X;
	
	//re-enable ADC0 SS0
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN1;

}

/// Method that receives ADC data
/** This method receives ADC data from the ADC.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
void adc_receive(void) 
{

	//initiate SS1 conversion
	ADC0_PSSI_R|=ADC_PSSI_SS1;
	
	//wait for ADC conversion to be complete
	while((ADC0_RIS_R & ADC_RIS_INR1) == 0){
		//wait
	}
	
	//grab result
	int value = ADC0_SSFIFO1_R;
	
	// Set averaging to remove jitters
	ADC0_SAC_R = 0x04;

}

/// Get a reading from a given ADC channel
/** This method reads a value from the ADC.
 * @param The ADC channel to read from.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
unsigned ADC_read(char channel)
{

	//disable ADC0SS0 sample sequencer to configure it
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN1;
	
	//set 1st sample to use the channel ADC pin
	ADC0_SSMUX1_R |= channel;
	
	//re-enable ADC0 SS0
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN1;
	
	//initiate SS0 conversion
	ADC0_PSSI_R=ADC_PSSI_SS1;
	
	//wait for ADC conversion to be complete
	while((ADC0_RIS_R & ADC_RIS_INR1) == 0){
		
	}
	
	//clear interrupt
	ADC0_ISC_R=ADC_ISC_IN1;
	
	// Return result
	return ADC0_SSFIFO1_R;

}

/// Conversion between quantization number to distance reading
/** This method converts the quatization number read from the ADC into a distance value.
 * @author Matthew Dickey, Yung-Hsueh Lee, Matthew Orth, and Katelyn Perkins
 * @date 4/12/2018
 */
double convert_distance(int quantization)
 {

    return 201480 * pow(quantization, -1.254);
	
}
