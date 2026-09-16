/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */

#include "main.h"
#include "stm32f0xx.h"

/*
	ADC CONFIGURATION Default Except:
	- Resolution: 8 bits
	- Channel: 5 (PA5)
*/

void Delay_ms(uint32_t ms) {
	const uint32_t cycles_per_ms = 8000 / 10;
	for (uint32_t i = 0; i < ms * cycles_per_ms; i++)
		__NOP();
}

void Init_Pot_GPIO() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
	GPIOA->MODER |= GPIO_MODER_MODER5; // Set PA5 to analog mode (11)
}

void Init_LED_GPIO() {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB clock
	GPIOB->MODER |= 0x5555u;		   // Set PB0-PB7 to output mode
}

void Write_LED(uint8_t value) {
	GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | (value & 0xFFu); // Write value to PB0-PB7
}

/**
 * @brief ADC Initialization Routine
 *
 * @note The many checks and conditions in this function are specified by the STM32F0 Ref
 * Manual. See:
 * 		- Section 13.3.2 "Calibration (ADCAL)""
 * 		- Section 13.3.3 "ADC on-off control (ADEN, ADDIS, ADRDY)"
 * 		- Section 13.3.5 "Configuring the ADC"
 * 		- Section 13.3.6 "Channel selection (CHSEL, SCANDIR)"
 *
 * @warning The ADC does not have enforce in hardware against misconfiguration.
 * 	Most registers specify specific conditions that must be met before writing to them.
 *  Not following these conditions can lead to undefined behavior.
 *  It is essential to refer closely to the reference manual at each step of the ADC
 *  configuration process.
 */
void Init_ADC() {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC1 clock

	/* ADC Configuration */
	if (ADC1->CR & ADC_CR_ADEN) { Error_Handler(); } // Error: ADC is already enabled
	ADC1->CFGR1 |= ADC_CFGR1_RES_1;					 // Set resolution to 8 bits

	if (ADC1->CR & ADC_CR_ADSTART) { Error_Handler(); } // Error: ADC has already started
	ADC1->CHSELR = ADC_CHSELR_CHSEL5;					// Select channel 5 (PA5)

	/* Calibration Procedure */
	if (ADC1->CR & ADC_CR_ADEN || ADC1->CR & ADC_CFGR1_DMAEN) {
		// Error: ADC must be disabled and DMA must be disabled for calibration
		Error_Handler();
	}
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL) {
		__NOP(); // Wait for Calibration to Complete
	}

	/* Enable the ADC */
	if (ADC1->CR != 0) { Error_Handler(); } // Error: All ADC control bits must be 0
	ADC1->CR |= ADC_CR_ADEN;				// Enable the ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
		__NOP(); // Wait for ADC to be ready
	}
}

void Start_ADC_Conversion() {
	if ((ADC1->CR & ADC_CR_ADEN)
		&& !(ADC1->CR & ADC_CR_ADDIS)) { // Ensure ADC is enabled and not disabled
		ADC1->CR |= ADC_CR_ADSTART;		 // Start the ADC conversion
	} else {
		Error_Handler(); // Error: ADC is not in a state to start conversion
	}
}

uint8_t Is_ADC_Conversion_Complete() {
	return (ADC1->ISR & ADC_ISR_EOC) != 0; // Check if the End of Conversion flag is set
}

uint8_t Read_ADC_Value() {
	while (!Is_ADC_Conversion_Complete()) {
		__NOP(); // Wait for conversion to complete
	}

	uint32_t adc_data_raw  = ADC1->DR; // Read the ADC data register
	uint8_t	 adc_data_8bit = (uint8_t)((adc_data_raw >> 0) & 0xFF);

	return adc_data_8bit;
}

int main(void) {
	Init_Pot_GPIO();
	Init_LED_GPIO();
	Init_ADC();

	uint8_t adc_value = 0;

	while (1) {
		Delay_ms(100);			// Delay for 100 milliseconds
		Start_ADC_Conversion(); // Start a new single conversion
		adc_value = Read_ADC_Value();
		Write_LED(adc_value); // Write the ADC value to the LEDs
	}
}

void Error_Handler(void) {

	while (1) {}
}
