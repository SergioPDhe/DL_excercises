#include "stm32f4xx_hal_i2c.h"

#define WRITE_ADDR 0xC0 // assume pins A0 and A1 are 0 (addresses from Table 5 in datasheet)
#define READ_ADDR  0xC1

#define CTRL_ADDR 0x00
#define FAULT_ADDR 0x01

#define FORWARD 0b01
#define REVERSE 0b10
#define BRAKE 	0b11
#define MAX_VOLTAGE	0x3F

// I2C instance is hi2c1

int MotorStop()
{
	uint8_t data = BRAKE | (MAX_VOLTAGE<<2);
	HAL_I2C_Mem_Write(&hi2c1, WRITE_ADDR, CTRL_ADDR, 1, &data, 1, 250);

	return 0;
}

int MotorStart(float speedPercent)
{

	uint8_t data = 0x00, velocity = 0x00, direction = FORWARD;


	if(speedPercent < 0) // if speed is negative
	{
		speedPercent *= -1; // convert to positive speed
		direction = REVERSE;		// motor is reversing
	}

	if(speedPercent > 100.0) speedPercent = 100.0; // cap value at 100%

	speedPercent /= 100; // convert 0-100% range to 0-1
	velocity = speedPercent * MAX_VOLTAGE;

	if(velocity <= 0x05) // reserved values
	{
		return MotorStop(); // consider low values as a speed of 0 and stop motor
	}

	data = (direction | (velocity << 2));
	HAL_I2C_Mem_Write(&hi2c1, WRITE_ADDR, CTRL_ADDR, 1, &data, 1, 0);

	return 0;
}

/// PART B ///////////////////////////////////////////////////////////////

void HAL_GPIO_EXTI_CALLBACK(uint16_t GPIO_Pin) // pin connected to Fault pin on motor driver IC. Interrupt set on falling edge.
{
	uint8_t faultData = 0x00, falutClear = (1<<7);

	HAL_I2C_Mem_Read(&hi2c1, READ_ADDR, FAULT_ADDR, 1, &faultData, 1, 250); // read fault register to faultData
	MotorStop();
	HAL_I2C_Mem_Write(&hi2c1, WRITE_ADDR, FAULT_ADDR, 1, &faultClear, 1, 250); // clear fault by setting bit 7

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_pin);

}
