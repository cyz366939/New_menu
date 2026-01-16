#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"
#include "OLED.h"
#include "Delay.h"
#include "Key_multi.h"
#include <string.h>
#include <stdlib.h>  //用于abs()函数

void ADC1_Init(void);
void show_adc_display(void);

#endif

