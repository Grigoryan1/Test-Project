
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "main.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

//Connect ultrasonic Echo otput to GPIOB Pin2
//Connect ultrasonic Trig otput to GPIOA Pin1
//USART Tx PB6
//USART Rx PB7


RTC_TimeTypeDef Time;
RTC_DateTypeDef Date;



int Adc_Result=0;
float Sum=0;
float Temp_array[8]={0};
float Temp_Display=0;
volatile uint32_t Us_Signal=0;
uint8_t Button_count=0;
volatile uint8_t Button_Flag=1;
uint8_t ADC_count=0;

void Systick_Init(void);
void vTask1 (void *pvParameters);
void vTask2 (void *pvParameters);
void vTask3 (void *pvParameters);
void vTask4 (void *pvParameters);
void LED_Init(void);
void Button_Init(void);
void USART1_Init(void);
void RTC_Init_U(void);
void ADCtemp_Init(void);
void Send_String(USART_TypeDef* USARTx, char *buffer);
float ADC_Read(void);	
float Average_ADC(void);
void Timers_Init(void);
void EchoPin_Init(void);

int main(void) {
	
	
	
	SystemInit();
	Systick_Init();
	LED_Init();
	RTC_Init_U();
  EchoPin_Init();
	Timers_Init();
	Button_Init();
	ADCtemp_Init();
	USART1_Init();

	
	
	 xTaskCreate(vTask1,"Led1",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
   
   xTaskCreate(vTask2,"Led2",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
						
	xTaskCreate(vTask3,"Led3",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);	

  xTaskCreate(vTask4,"Led4",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
   
   vTaskStartScheduler(); // Start the scheduler. 
	
	while(1)
	{}
	
	
}

void vTask1 (void *pvParameters)
{
   for(;;)
  {
		char buf[16]={0};
			portTickType lastExecutionTime = xTaskGetTickCount();
				
		RTC_GetTime(RTC_Format_BIN, &Time);
		RTC_GetDate(RTC_Format_BIN, &Date);
			
		
		
		
	  sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d %.1f",Date.RTC_Year, Date.RTC_Month, Date.RTC_Date,  
						Time.RTC_Hours,  Time.RTC_Minutes, Time.RTC_Seconds, Temp_Display);
		if(Button_Flag==1){
		switch(Date.RTC_WeekDay)
		{
				case 1:
					Send_String(USART1, "MON ");
						break;
					case 2:
						Send_String(USART1, "TUE ");
							break;
						case 3:
							Send_String(USART1, "WED ");
								break;
							case 4:
								Send_String(USART1, "THU ");
									break;
								case 5:
									Send_String(USART1, "FRI ");
										break;
									case 6:
										Send_String(USART1, "SAT ");
											break;
										case 7:
											Send_String(USART1, "SUN ");
												break;
										default:
											Send_String(USART1, "___");
												break;
											
		}	
		Send_String(USART1, buf);
		Send_String(USART1, "\n");
		//Send_String(USART1, "Data\n");
		}
	

    vTaskDelayUntil(&lastExecutionTime, 1000);

    
	}
}

void vTask2 (void *pvParameters)
{
	 for(;;)
  {

		if(ADC_count<8){
		Sum+=ADC_Read();
		ADC_count++;
		}else
		{
			Temp_Display=Sum/8;
			Sum=0;
			ADC_count=0;
		}
  
    vTaskDelay(100);
	}
}

void vTask3 (void *pvParameters)
{
	 for(;;)
  {
		float a=(float)Us_Signal/58;
	  if(a<=500){
			GREEN_ON;
			YELLOW_OFF;
			RED_OFF;
			BLUE_OFF;
		}else if(500<a && a<=1000){
			GREEN_ON;
			YELLOW_ON;
			RED_OFF;
			BLUE_OFF;
		}else if(1000<a && a<=1500){
			GREEN_ON;
			YELLOW_ON;
			RED_ON;
			BLUE_OFF;
		}else if(1500<a && a<=2000){
			GREEN_ON;
			YELLOW_ON;
			RED_ON;
			BLUE_ON;
		}
    vTaskDelay(200);
	}
}

void vTask4 (void *pvParameters)
{
	 for(;;)
  {
		if(READ_BUTTON==1)
		{
			Button_count++;
			if(Button_count>=10)
			{
				Button_count=0;
				if(Button_Flag==1){
					Button_Flag=0;
				}else{
					Button_Flag=1;
				}
			}
		}else{
			Button_count=0;
		}
		
    vTaskDelay(5);
	}
}




void Send_String(USART_TypeDef* USARTx, char *buffer)
{
	while (*buffer != 0){
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)== RESET){}
		USART_SendData(USARTx, *buffer++);
	}
}
	
float ADC_Read()
{
	 float Temperature=0;
	 ADC_SoftwareStartConv(ADC1);
   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){}
   Adc_Result= ADC_GetConversionValue(ADC1);
	 Temperature=(((double)Adc_Result*2.9/4095-0.76)*1000/2.5)+25;
	 
	 return Temperature;
}	

float Average_ADC()
{

	
	
return 1;
}


void Systick_Init()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}

void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Button_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void USART1_Init()
{

		USART_InitTypeDef Usart1;
 
		GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
 
    GPIO_StructInit(&GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
 
		USART_StructInit(&Usart1);
		Usart1.USART_Mode = USART_Mode_Tx;
		Usart1.USART_BaudRate = 9600;	
		USART_Init(USART1, &Usart1);	
 
		NVIC_EnableIRQ(USART1_IRQn);
		USART_Cmd(USART1, ENABLE);
		
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
}

void RTC_Init_U()
{
		RTC_InitTypeDef RTC_InitStructure;
		
	  
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
 
    PWR_BackupAccessCmd(ENABLE);
 
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
 
    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div8);
    RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
	
		RTC_InitStructure.RTC_AsynchPrediv = 124;
		RTC_InitStructure.RTC_SynchPrediv  = 7999; 
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);
 
		Date.RTC_Year = 16;
		Date.RTC_Month = RTC_Month_May;
		Date.RTC_Date = 29;
		Date.RTC_WeekDay = RTC_Weekday_Sunday;
		RTC_SetDate(RTC_Format_BIN, &Date);
 
		Time.RTC_H12     = RTC_H12_PM;
		Time.RTC_Hours   = 13;
		Time.RTC_Minutes = 32;
		Time.RTC_Seconds = 0;
		RTC_SetTime(RTC_Format_BIN, &Time);
	
}



void ADCtemp_Init()
{
		ADC_CommonInitTypeDef Adc1;
		ADC_InitTypeDef ADC_InitStructure;
	
    ADC_StructInit(&ADC_InitStructure);
    ADC_CommonStructInit(&Adc1);
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    Adc1.ADC_Mode = ADC_Mode_Independent;
    Adc1.ADC_Prescaler = ADC_Prescaler_Div2;
       
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
 
     ADC_CommonInit(&Adc1);
     ADC_Init(ADC1, &ADC_InitStructure);
     ADC_Cmd(ADC1, ENABLE);
		 ADC_TempSensorVrefintCmd(ENABLE); 
		 ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_15Cycles);
}

void Timers_Init()
{		
		//Connect ultrasonic Trig otput to GPIOA Pin1
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_InitStructure;
		TIM_OCInitTypeDef timerPWM;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3, ENABLE);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
		
		TIM_TimeBaseStructInit(&TIM_InitStructure); 
		TIM_InitStructure.TIM_Prescaler = 41;    
		TIM_InitStructure.TIM_Period = 199999; 
		TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
		TIM_Cmd(TIM2, ENABLE); 
 
    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_Pulse = 10;
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
		timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
		
    TIM_OC2Init(TIM2, &timerPWM);
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
		TIM_Cmd(TIM2, ENABLE);
		
				
		TIM_TimeBaseStructInit(&TIM_InitStructure); 
		TIM_InitStructure.TIM_Prescaler = 41;    
		TIM_InitStructure.TIM_Period = 50000;        
		TIM_TimeBaseInit(TIM3, &TIM_InitStructure);

}

void EchoPin_Init()
{
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef exti;
  GPIO_InitTypeDef GPIO_InitStructure; 
	__enable_irq();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   
  // Set GPIOB Pin2 EXTI interrupt sourse  
	GPIO_StructInit(&GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
 
	// EXTI interrupt
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	exti.EXTI_Line = EXTI_Line2;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler()
{
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_TC);		
    }
}  

void EXTI2_IRQHandler()
{
  if (EXTI_GetITStatus(EXTI_Line2) != RESET){
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==1){
			TIM_Cmd(TIM3, ENABLE);
		}
		else{
				TIM_Cmd(TIM3, DISABLE); 
				Us_Signal=TIM_GetCounter(TIM3);
				TIM_SetCounter(TIM3, 0);			
		}
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
  
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  
  while (1)
  {
  }
}
#endif
