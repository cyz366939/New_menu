#ifndef DHT11_H_
#define DHT11_H_
#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"

#define DHT11_PIN GPIO_Pin_5
#define DHT11_GPIO_PORT GPIOB

typedef struct
{
	uint8_t humidity_int;	 // 湿度整数
	uint8_t humidity_dec;	 // 湿度小数
	uint8_t temperature_int; // 温度整数
	uint8_t temperature_dec; // 温度小数
	uint8_t check_sum;		 // 校验和
} DHT11_t;
extern DHT11_t DHT11_Data;

void DHT11_Init(void);
uint8_t DHT11_Read_Data(DHT11_t *pDHT11);
uint8_t DHT11_Read_Float(float *temperature, float *humidity);

#endif /* DHT11_H_ */
