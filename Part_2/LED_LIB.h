#include "stm32f3xx_hal.h"

#define LED_NUM 100			// total number of LEDs
#define TOTAL_BITS	24		// total bits per LED

#define COUNTER_PERIOD 50	// counter period
#define CLOCK_MHZ 40		//clock speed in MHz

#define T1H_NS 800			// HIGH time of 800ns for logical 1
#define T0H_NS 400			// HIGH time of 400ns for logical 0
#define CYCLE_NS 1250		// length of duty cycle 1250ns

#define RESET_GAP	50		// 50 cycles for reset

#define T1H_COUNT ((COUNTER_PERIOD * T1H_NS) / CYCLE_NS) // how many counts of HIGH for logical 1? (32 / 50)
#define T0H_COUNT ((COUNTER_PERIOD * T0H_NS) / CYCLE_NS) // how many counts of HIGH for logical 0? (16 / 50)

uint8_t LEDs[LED_NUM][3];	// array with all LED data
uint32_t pwmData[(LED_NUM*TOTAL_BITS) + RESET_GAP];	// data to be sent through PWM - RESET_GAP represents LOW of >50us to reset data transmission
volatile int dataSent = 0; // flag to check when data is done sending


int LEDSendData() // send all data to LEDs
{
	uint32_t LEDData, pwmID=0;

	for(int i=0; i<LED_NUM; i++)
	{
		LEDData = (LEDs[i][0]<<16 | LEDs[i][1]<<8 | LEDs[i][2]);

		for(int j=(TOTAL_BITS-1); j>=0; j--)
		{
			if(LEDData&(1<<j))
				pwmData[pwmID] = T1H_COUNT; // 1
			else
				pwmData[pwmID] = T0H_COUNT; // 0
			pwmID++;
		}
	}

	for(int i=0; i<RESET_GAP; i++) // pull line to LOW for >50us
	{
		pwmData[pwmID]=0;
		pwmID++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, &pwmData, pwmID);
	while(!dataSent);
	dataSent = 0;
}

int SetRgbColor(int ID, uint8_t red, uint8_t green, uint8_t blue) // set RGB values for any LED
{
	LEDs[ID][0] = green;
	LEDs[ID][1] = red;
	LEDs[ID][2] = blue;

	LEDSendData();

	return 0;
}

int HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) // callback function when PWM is finished
{
	HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
	dataSent=1;
}
