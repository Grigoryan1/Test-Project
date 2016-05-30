
#ifndef __MAIN_H
#define __MAIN_H

#define READ_BUTTON 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

#define GREEN_ON			GPIO_SetBits(GPIOD,GPIO_Pin_12);
#define GREEN_OFF			GPIO_ResetBits(GPIOD,GPIO_Pin_12);

#define YELLOW_ON			GPIO_SetBits(GPIOD,GPIO_Pin_13);
#define YELLOW_OFF		GPIO_ResetBits(GPIOD,GPIO_Pin_13);

#define RED_ON				GPIO_SetBits(GPIOD,GPIO_Pin_14);
#define RED_OFF				GPIO_ResetBits(GPIOD,GPIO_Pin_14);

#define BLUE_ON				GPIO_SetBits(GPIOD,GPIO_Pin_15);
#define BLUE_OFF			GPIO_ResetBits(GPIOD,GPIO_Pin_15);

#include "stm32f4xx.h"

#endif 


