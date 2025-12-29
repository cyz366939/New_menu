#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"
#include "OLED.h"
#include "Delay.h"
#include "KEY.h"
#include <string.h>
#include "USART1.h"

#define ADC_BUFFER_SIZE  128// DMA缓冲区大小


void ADC_DualChannel_Init(void);
void Extract_ADC_Data(void);
float Calculate_Temperature(uint16_t temp_adc);
void wave_drain(void);
void show_temperature_only(void);
#endif

