/*
 * BattPreChg.c
 * For 8-up battery production precharging fixture
 * Designed to charge batteries up to a safe, nominal voltage such as 4.0V,
 * and then stop charging (no top off to 4.1V). 
 * Reset button re-initializes for a new set of 8 batteries.
 *
 * Created: 5/12/2021
 * Author : Deyo
 *
 */ 

#define F_CPU 16000000		// Must be defined before delay library

#include <avr/io.h>
#include <util/delay.h>

// ADC channels for 8 Vbatts
#define ADCbatt1		0x00		// ADC channel 0, MUX5:0(000000)
#define ADCbatt2		0x01		// ADC channel 1, MUX5:0(000001)
#define ADCbatt3		0x04		// ADC channel 4, MUX5:0(000100)
#define ADCbatt4		0x05		// ADC channel 5, MUX5:0(000101)
#define ADCbatt5		0x06		// ADC channel 6, MUX5:0(000110)
#define ADCbatt6		0x07		// ADC channel 7, MUX5:0(000111)
#define ADCbatt7		0x23		// ADC channel 11, MUX5:0(100011)
#define ADCbatt8		0x24		// ADC channel 12, MUX5:0(100100)

// Variables that may need to be changed
#define BLINK		250		// Blink rate for initialization
#define AVE_ADC		4		// Number of ADC readings averaged 
#define CHG_THRESH	820		// ADC reading above which charging is disabled, ADC = (Vbatt * 1023) / VCC; 820 bits = 4.0V

void initState(void);				// Initialize mode; ready for 8 new batteries
void selectADCchannel(uint8_t);		// Utility function 

uint8_t ADCchannel[8] = {ADCbatt1, ADCbatt2, ADCbatt3, ADCbatt4, ADCbatt5, ADCbatt6, ADCbatt7, ADCbatt8};	// ADC channels for battery, set up in array for loops
uint8_t BattChgDisableFlags;		// Bitfield array; when set to 1, charging is disabled		

int main()
{
	// Config system clock for external crystal
	CLKSEL0 = 0b00010101;
	CLKSEL1 = 0b00001111;  // Select external low power crystal
	
	// Config ports
	DDRB = 0b00000000;
	DDRC = 0b00000000;
	DDRD = 0b11111111;
	DDRE = 0b00000000;
	DDRF = 0b00000000;
	PORTD = 0b00000000;				// No pull-ups on CENx
	PORTC = 0b10000000;				// Pull-up on Reset switch
	
	// Config ADCs
	ADMUX |= (1 << REFS0);	 // Vref = AVCC on AREF pin
	ADCSRA |= (1 << ADEN);	 // Enable
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);	// Prescaler of 128 for 125kHz ADC clock
	DIDR0 |= 0b11110011;		// Disable digital input on analog pins
	DIDR2 |= 0b00011000;		// Disable digital input on analog pins
	
	uint8_t button;
	uint16_t BattVoltage;	
	
	// Initialize system
	initState();
	
	// Main loop
	while(1) {
		// Poll for Reset button push
		button = (PINC & (1 << PINC7)) >> PINC7;		// Get boolean state of button
		if (button == 0) initState();			// Reset to initialized state
		
		// Cycle through batteries, measuring and removing from queue as they reach CHG_THRESH
		for (int i = 0; i < 8; i++) {
			if (((BattChgDisableFlags >> i) & 0x01) == 0) {		// Measure if battery still in queue
				BattVoltage = 0;						// Get ready for averaging ADC measurements
				selectADCchannel(ADCchannel[i]);
			
				for (int j = 0; j < AVE_ADC; j++) {		// Get ADC values
					ADCSRA |= (1<<ADSC);
					while (ADCSRA & (1 << ADSC));		// Wait for interrupt
					BattVoltage = BattVoltage + ADC;
					_delay_ms(5);
				}
				// Remove battery from charging queue if averaged ADC is above threshold
				if ((BattVoltage / AVE_ADC) > CHG_THRESH) {
					BattChgDisableFlags |= (0b00000001 << i);		
					PORTD = BattChgDisableFlags;			// Disable charger/ blue LED on
				}
			}
		}
	}
}

void initState() {
	_delay_ms(BLINK);
	for(int i = 0; i < 8; i++) {			// cycle through blue LEDs
		PORTD = (0b00000001 << i);
		_delay_ms(BLINK);
	}
	BattChgDisableFlags = 0;		// Chargers all enabled, all batteries in queue
	PORTD = 0;					// all blue LEDs off, ready for main loop
}

void selectADCchannel(uint8_t chan) {
	ADMUX = (ADMUX & 0xE0) | (chan & 0x1F);		//select channel (MUX0-4 bits)
	ADCSRB = (ADCSRB & 0xDF) | (chan & 0x20);   //select channel (MUX5 bit)
}
