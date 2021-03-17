/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <max7219.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EEPROM_ADDR 0xA0
#define DEBUG false
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void read_settings_from_eeprom(void);
void logic_runner(void);
void stop_and_clear_tim1(void);
void serial_display_credit(void);
void set_add_duration_of_function(uint8_t _selected_menu);
void set_substract_duration_of_function(uint8_t _selected_menu);
void segment_display_off(void);
void enable_all_exti_it(void);
void disable_all_exti_it(void);
void iot_send_inserted_credit(uint16_t inst_credit);
void iot_send_mode(uint8_t mode);
void iot_send_status(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t DEV_ID=0;
uint32_t REV_ID=0;
volatile uint32_t IC_Val1 = 0;
volatile uint32_t IC_Val2 = 0;
volatile uint32_t Difference = 0;
volatile uint8_t Is_First_Captured = 0;  // is the first value captured ?
uint8_t default_credit_duration = 10;
volatile uint32_t coin_IC_Val1 = 0;
volatile uint32_t coin_IC_Val2 = 0;
volatile uint32_t coin_Difference = 0;
volatile uint8_t coin_Is_First_Captured = 0;  // is the first value captured ?
volatile uint8_t selected_menu = 0;

volatile uint32_t last_event_credit = 0;
volatile uint32_t credit = 0;
volatile uint8_t selected_button = 0;
volatile int logic_runner_round_counter = 0;
volatile int minimum_credit_to_start = 10;
volatile int duration_per_1credit = 5;
volatile bool front_btn_enable = false;
volatile bool system_function_start = false;
volatile bool displayEnable = true;
volatile bool displayToggle = true;
/// coin and bank note global

volatile uint8_t bank_acceptor_pulse_width = 33;
volatile uint8_t coin_acceptor_pulse_width = 40;
uint8_t creditPulseOffset = 15;

volatile uint8_t bank_credit_per_pulse = 10;
volatile uint8_t coin_credit_per_pulse = 1;
/// coin and bank note global var

/* duration settings */
volatile uint8_t F1_DURATION = 10;
volatile uint8_t F2_DURATION = 10;
volatile uint8_t F3_DURATION = 10;
volatile uint8_t F4_DURATION = 10;
volatile uint8_t F5_DURATION = 10;

volatile int8_t current_setting_value = 0;
volatile uint8_t selected_setting = 0;
volatile uint16_t TIM1_COUNTER = 0;
volatile uint32_t burst_bank_credit = 0;
volatile uint32_t burst_coin_credit = 0;
volatile uint32_t burst_counter = 0;
volatile uint32_t burst_counter_2 = 0;
volatile bool start_debounce_counter = false;
volatile bool start_debounce_counter2 = false;
volatile uint32_t lastTimePress = 0;
volatile uint32_t lastCoinTick = 0;
char dev_id_buffer[30];
uint16_t last_credit_insert = 0;
uint16_t last_credit_insert_bank = 0;
char newline[] = "\r\n";
volatile char buffer[10];

volatile uint8_t data_to_write = 14;
volatile char data_to_read[20];

volatile uint8_t data_read = 0;
volatile uint8_t standby_counter = 0;
volatile bool clearButton = false;
volatile uint8_t clearButtonCounter = 0;
volatile uint8_t tim2_round_counter = 0;
volatile uint8_t iot_round_counter = 0;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_SPI2_Init();
	MX_RTC_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_Delay(400);
	read_settings_from_eeprom();
	HAL_Delay(400);
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim4);
	HAL_TIM_Base_Stop_IT(&htim1);
	max7219_DisableDisplayTest();
	HAL_Delay(400);
	max7219_Init ( 5 );
	max7219_Decode_On ();
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim2);
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	DEV_ID =  HAL_GetDEVID();
	REV_ID = HAL_GetREVID();

	sprintf(dev_id_buffer, "$DVID%d-%d&\r\n",(int)DEV_ID,(int)REV_ID);
	HAL_UART_Transmit(&huart1, (uint8_t *)dev_id_buffer, strlen(dev_id_buffer), HAL_MAX_DELAY);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_Delay(1000);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)){
			credit = 0;
			system_function_start = false;
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void read_settings_from_eeprom(void){
	F1_DURATION = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
	F2_DURATION = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
	F3_DURATION = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
	F4_DURATION = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
	F5_DURATION = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
	credit = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
	if(credit > 2){
		system_function_start = true;
	}else{
		credit = 0;
	}
	char tmp_msg[35];
	sprintf(tmp_msg,"eeprom read DURATION 1 is %d \r\n",F1_DURATION);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
	sprintf(tmp_msg,"eeprom read DURATION 2 is %d \r\n",F2_DURATION);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
	sprintf(tmp_msg,"eeprom read DURATION 3 is %d \r\n",F3_DURATION);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
	sprintf(tmp_msg,"eeprom read DURATION 4 is %d \r\n",F4_DURATION);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
	sprintf(tmp_msg,"eeprom read DURATION 5 is %d \r\n",F5_DURATION);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
	sprintf(tmp_msg,"eeprom read CREDIT is %d \r\n",(int)credit);
	HAL_UART_Transmit(&huart1, (uint8_t *)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);

	if(F1_DURATION == 0 || F2_DURATION == 0 || F3_DURATION == 0 || F4_DURATION == 0 || F5_DURATION == 0){
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, default_credit_duration);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, default_credit_duration);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, default_credit_duration);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, default_credit_duration);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, default_credit_duration);
		F1_DURATION = default_credit_duration;
		F2_DURATION = default_credit_duration;
		F3_DURATION = default_credit_duration;
		F4_DURATION = default_credit_duration;
		F5_DURATION = default_credit_duration;
	}
	HAL_Delay(300);
}
void reset_all_output(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // pin b0 --> out 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // pin b1 --> out 2
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // pin a8 --> out 3
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // pin b8 --> out 4
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // pin b9 --> out 5
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // pin b12 --> out 6
}
void set_output_to(uint8_t pin){
	switch(pin){
	case 3:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // pin b0 --> out 1
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // pin b1 --> out 2
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // pin a8 --> out 3
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // pin b8 --> out 4
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // pin b9 --> out 5
		break;
	default:
		reset_all_output();
		max7219_Turn_On();
		break;
	}

}

void segment_display_int(int number){
	max7219_Decode_On();
	max7219_Clean ();
	if(number < 10){
		max7219_PrintItos (DIGIT_1, number );
	}else if(number >= 10 && number < 100){
		max7219_PrintItos (DIGIT_2, number );
	}else{
		max7219_PrintItos (DIGIT_3, number );
	}
}
void segment_display_function_settings(int func_number,int value){
	max7219_Decode_On();
	max7219_Clean ();
	if(value < 10){
		max7219_PrintItos (DIGIT_1, value );
	}else if(value >= 10 && value < 100){
		max7219_PrintItos (DIGIT_2, value );
	}
	max7219_PrintDigit(DIGIT_3,func_number,true);
}
void segment_display_standby(){
	max7219_Clean ();
	max7219_Decode_On();
	max7219_Clean ();
	if(displayEnable == true){
		switch(standby_counter){
		case 0:
			max7219_PrintDigit(DIGIT_1,MINUS,false);
			max7219_PrintDigit(DIGIT_2,BLANK,false);
			max7219_PrintDigit(DIGIT_3,BLANK,false);
			break;
		case 1:
			max7219_PrintDigit(DIGIT_1,BLANK,false);
			max7219_PrintDigit(DIGIT_2,MINUS,false);
			max7219_PrintDigit(DIGIT_3,BLANK,false);
			break;
		case 2:
			max7219_PrintDigit(DIGIT_1,BLANK,false);
			max7219_PrintDigit(DIGIT_2,BLANK,false);
			max7219_PrintDigit(DIGIT_3,MINUS,false);
			break;
		case 3:
			max7219_PrintDigit(DIGIT_1,BLANK,false);
			max7219_PrintDigit(DIGIT_2,MINUS,false);
			max7219_PrintDigit(DIGIT_3,BLANK,false);
			break;
		case 4:
			max7219_PrintDigit(DIGIT_1,MINUS,false);
			max7219_PrintDigit(DIGIT_2,BLANK,false);
			max7219_PrintDigit(DIGIT_3,BLANK,false);
			break;
		case 5:
			max7219_PrintDigit(DIGIT_1,MINUS,false);
			max7219_PrintDigit(DIGIT_2,MINUS,false);
			max7219_PrintDigit(DIGIT_3,MINUS,false);
			break;
		case 6:
			max7219_PrintDigit(DIGIT_1,MINUS,false);
			max7219_PrintDigit(DIGIT_2,MINUS,false);
			max7219_PrintDigit(DIGIT_3,MINUS,false);
			break;
		default:
			max7219_PrintDigit(DIGIT_1,MINUS,false);
			max7219_PrintDigit(DIGIT_2,MINUS,false);
			max7219_PrintDigit(DIGIT_3,MINUS,false);
			break;
		}
		if(standby_counter >= 6){
			standby_counter = 0;
		}else{
			standby_counter += 1;
		}
		//		max7219_PrintDigit(DIGIT_1,MINUS,false);
		//		max7219_PrintDigit(DIGIT_2,MINUS,false);
		//		max7219_PrintDigit(DIGIT_3,MINUS,false);
		displayEnable = false;
	}else{
		max7219_PrintDigit(DIGIT_1,BLANK,false);
		max7219_PrintDigit(DIGIT_2,BLANK,false);
		max7219_PrintDigit(DIGIT_3,BLANK,false);
		displayEnable = true;
	}

}
void segment_display_off(){
	max7219_Clean ();
	max7219_Decode_On();
	max7219_Clean ();
	max7219_PrintDigit(DIGIT_1,BLANK,false);
	max7219_PrintDigit(DIGIT_2,BLANK,false);
	max7219_PrintDigit(DIGIT_3,BLANK,false);
}

void iot_send_inserted_credit(uint16_t inst_credit){
	char cmd_buffer[35];
	sprintf(cmd_buffer,"%cINST_%d_%d%c\r\n",0x02,(unsigned int)credit,(unsigned int)inst_credit,0x03);
	if(DEBUG){
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}
}
void iot_send_mode(uint8_t mode){
	char cmd_buffer[35];
	sprintf(cmd_buffer,"%cMODE_%d_%d%c\r\n",0x02,(unsigned int)credit,(unsigned int)mode,0x03);
	if(DEBUG){
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}
}
void iot_send_status(){
	char cmd_buffer[35];
	sprintf(cmd_buffer,"%cSTAT_%d%c\r\n",0x02,(unsigned int)credit,0x03);
	if(DEBUG){
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart1, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, (uint8_t *)cmd_buffer, strlen(cmd_buffer), HAL_MAX_DELAY);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4) { /// tick every 1s
		max7219_Turn_On();
		//		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 2);
		//		char tmp[25];
		//		uint8_t mapped_value = calculate_adc(adcBuffer[0]);
		if(clearButton == true){
			if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 1){
				HAL_UART_Transmit(&huart1, "---> CLEAR BTN >>PRESS<< !!!\r\n", 30, HAL_MAX_DELAY);
				selected_button = 0;
				reset_all_output();
				clearButtonCounter += 1;
			}else{
				clearButton = false;
				clearButtonCounter = 0;
				HAL_TIM_Base_Stop_IT(&htim4);
			}
			if(clearButtonCounter > 5){
				HAL_UART_Transmit(&huart1, "---> CLEAR CREDITS !!!\r\n", 24, HAL_MAX_DELAY);
				clearButton = false;
				clearButtonCounter = 0;
				credit = 0;
				system_function_start = false;
				selected_button = 0;
				reset_all_output();
				clearButtonCounter = 0;
				HAL_TIM_Base_Stop_IT(&htim4);
			}
		}
		if (selected_menu > 0) {
			HAL_TIM_Base_Stop_IT(&htim2); //stop main program
			switch (selected_menu) {
			case 1:
				//				max7219_PrintDigit(DIGIT_3,16,true);
				segment_display_function_settings(selected_menu,F1_DURATION);
				//				max7219_PrintDigit(DIGIT_2,NUM_1,false);
				//				memset(tmp, 0, sizeof tmp);
				//				sprintf(tmp, "F1 VALUE : %d \r\n", (unsigned int) mapped_value);
				//				HAL_UART_Transmit(&huart1, tmp, strlen(tmp), HAL_MAX_DELAY);
				//				F1_DURATION = mapped_value;
				break;
			case 2:
				segment_display_function_settings(selected_menu,F2_DURATION);
				break;
			case 3:
				max7219_PrintDigit(DIGIT_3,18,true);
				segment_display_function_settings(selected_menu,F3_DURATION);
				break;
			case 4:
				max7219_PrintDigit(DIGIT_3,19,true);
				segment_display_function_settings(selected_menu,F4_DURATION);
				break;
			case 5:
				segment_display_function_settings(selected_menu,F5_DURATION);
				break;
			}
		} else {
			max7219_Decode_On();
			HAL_TIM_Base_Start_IT(&htim2);
			if(clearButton == false){
				HAL_TIM_Base_Stop_IT(&htim4);
			}
		}
	}
	if (htim->Instance == TIM2) { /// tick every 1ms
		//		HAL_UART_Transmit(&huart1, (uint8_t*)"TIM2 TICK!!\r\n", 13, HAL_MAX_DELAY);
		char credit_tmp_msg[55];
		if(last_credit_insert > 0){
			sprintf(credit_tmp_msg,"last credit inserted: %d \r\n",(unsigned int)last_credit_insert);
			iot_send_inserted_credit(last_credit_insert);
			HAL_UART_Transmit(&huart1,(uint8_t *)credit_tmp_msg, strlen(credit_tmp_msg), HAL_MAX_DELAY);
			last_credit_insert = 0;
		}
		if(last_credit_insert_bank > 0){
			sprintf(credit_tmp_msg,"last bank credit inserted: %d \r\n",(unsigned int)last_credit_insert_bank);
			iot_send_inserted_credit(last_credit_insert_bank);
			HAL_UART_Transmit(&huart1,(uint8_t *)credit_tmp_msg, strlen(credit_tmp_msg), HAL_MAX_DELAY);
			last_credit_insert_bank = 0;
		}
		if(tim2_round_counter >= 10){
			HAL_UART_Transmit(&huart1, (uint8_t*)"TIM2 TICK!!\r\n", 13, HAL_MAX_DELAY);
			logic_runner();
			tim2_round_counter = 0;
			if(iot_round_counter > 30){
				iot_send_status();
				iot_round_counter = 0;
			}else{
				iot_round_counter += 1;
			}
		}else{
			if((tim2_round_counter%3) == 0){
				if (selected_button != 0) {
					if(displayToggle){
						max7219_Turn_On();
						displayToggle = false;
					}else{
						max7219_Turn_Off();
						displayToggle = true;
					}
				}else{
					max7219_Turn_On();
				}
			}
			if(credit <= 0){
				if((tim2_round_counter&2) == 0){
					segment_display_standby();
				}
			}

			tim2_round_counter += 1;

		}
	}
	if (htim->Instance == TIM1) { /// tick every 1000ms
		stop_and_clear_tim1();
		//		enable_all_exti_it();
		HAL_UART_Transmit(&huart1, (uint8_t*)"TIM SW DEBOUNCE TICK!\r\n", 23, HAL_MAX_DELAY);
		char tmp[25];
		sprintf(tmp, "selected mode %d \r\n", (unsigned int) selected_button);
		reset_all_output();
		set_output_to(selected_button);
		iot_send_mode(selected_button);
		HAL_UART_Transmit(&huart1, (uint8_t*)tmp, strlen(tmp), HAL_MAX_DELAY);
		if(selected_menu <= 0){
			HAL_TIM_Base_Start_IT(&htim2);
		}

	}
}
void disable_all_exti_it(){
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}
void enable_all_exti_it(){
	HAL_NVIC_ClearPendingIRQ(EXTI2_IRQn);
	HAL_NVIC_ClearPendingIRQ(EXTI3_IRQn);
	HAL_NVIC_ClearPendingIRQ(EXTI4_IRQn);
	HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	disable_all_exti_it();
	char message_sw2[] = "Switch [2] pressed!\r\n";
	char message_sw3[] = "FRONT SW [1] pressed!\r\n";
	char message_sw4[] = "FRONT SW [2] pressed!\r\n";
	char message_sw5[] = "FRONT SW [3] pressed!\r\n";
	char message_sw6[] = "FRONT SW [4] pressed!\r\n";
	char message_sw7[] = "FRONT SW [5] pressed!\r\n";
	uint32_t tickstart = HAL_GetTick();
	char tick_msg[35];
	sprintf(tick_msg,"---- sysTick : %d\r\n",tickstart);
	if( (tickstart - lastTimePress)<=50){
		HAL_UART_Transmit(&huart1,"too many times press !!!!\r\n", 27,HAL_MAX_DELAY);
		enable_all_exti_it();
		return;
	}else{
		lastTimePress = tickstart;
	}
	max7219_DisableDisplayTest();
	HAL_UART_Transmit(&huart1, (uint8_t*)tick_msg, strlen(tick_msg),
			HAL_MAX_DELAY);
	if (GPIO_Pin == GPIO_PIN_2 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == 1) {
		enable_all_exti_it();
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_UART_Transmit(&huart1, (uint8_t*)message_sw2, strlen(message_sw2),
				HAL_MAX_DELAY);
		selected_menu += 1;
		HAL_TIM_Base_Start_IT(&htim4);
		if (selected_menu > 5) {
			selected_menu = 0;
			HAL_TIM_Base_Stop_IT(&htim4);
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}
	if(selected_menu > 0){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 1) {
			set_add_duration_of_function(selected_menu);
		}
		else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 1) {
			set_substract_duration_of_function(selected_menu);
		}
	}
	else if (system_function_start) {
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_Base_Stop_IT(&htim1);
		__HAL_TIM_SET_COUNTER(&htim1, 0);
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		displayToggle = true;
		max7219_Turn_On();
		switch (GPIO_Pin) {
		case GPIO_PIN_3:
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t*)message_sw3, strlen(message_sw3),
						HAL_MAX_DELAY);
				if (selected_button == 3) {
					selected_button = 0;
					reset_all_output();
				} else {
					reset_all_output();
					selected_button = 3;
				}
			}
			break;
		case GPIO_PIN_4:
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t*)message_sw4, strlen(message_sw3),
						HAL_MAX_DELAY);
				if (selected_button == 4) {
					selected_button = 0;
					reset_all_output();
				} else {
					reset_all_output();
					selected_button = 4;
				}
			}
			break;
		case GPIO_PIN_5:
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t*)message_sw5, strlen(message_sw3),
						HAL_MAX_DELAY);
				if (selected_button == 5) {
					selected_button = 0;
					reset_all_output();
				} else {
					reset_all_output();
					selected_button = 5;
				}
			}
			break;
		case GPIO_PIN_6:
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t*)message_sw6, strlen(message_sw3),
						HAL_MAX_DELAY);
				if (selected_button == 6) {
					reset_all_output();
					selected_button = 0;
				} else {
					reset_all_output();
					selected_button = 6;
				}
			}
			break;
		case GPIO_PIN_7:
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t*)message_sw7, strlen(message_sw3),
						HAL_MAX_DELAY);
				if (selected_button == 7) {
					selected_button = 0;
					reset_all_output();
					clearButton = true;
					HAL_TIM_Base_Start_IT(&htim4);
				} else {
					reset_all_output();
					selected_button = 7;
				}
			}
			break;
		}
	}
	//	uint32_t i = 100000;
	//	while (i-- > 0) {
	//		asm("nop");
	//	}
	enable_all_exti_it();
	HAL_TIM_Base_Start_IT(&htim1);
}
void eeprom_write(uint8_t addr, uint8_t data){
	/*
	 * EEPROM ADDRESS TABLE
	 * ------------------------
	 *  address | description
	 * ------------------------
	 *   0x01   |  F1_DURATION
	 *   0x02   |  F2_DURATION
	 *   0x03   |  F3_DURATION
	 *   0x04   |  F4_DURATION
	 *   0x05   |  F5_DURATION
	 *
	 *	 0x06   |  credit
	 *
	 * */
	switch(addr){
	case 0x01:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, data);
		break;
	case 0x02:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, data);
		break;
	case 0x03:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, data);
		break;
	case 0x04:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, data);
		break;
	case 0x05:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, data);
		break;
	case 0x06:
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, data);
		break;
	}

}
//uint8_t eeprom_read(uint8_t addr){
//	uint8_t recv_data;
//	HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, addr, 0xFF, &recv_data, 1, HAL_MAX_DELAY);
//}
void set_add_duration_of_function(uint8_t _selected_menu){
	if(_selected_menu > 0){
		switch (_selected_menu) {
		case 1:
			if(F1_DURATION < 99){
				F1_DURATION += 1;
				eeprom_write(0x01,F1_DURATION);
			}
			break;
		case 2:
			if(F2_DURATION < 99){
				F2_DURATION += 1;
				eeprom_write(0x02,F2_DURATION);
			}
			break;
		case 3:
			if(F3_DURATION < 99){
				F3_DURATION += 1;
				eeprom_write(0x03,F3_DURATION);
			}
			break;
		case 4:
			if(F4_DURATION < 99){
				F4_DURATION += 1;
				eeprom_write(0x04,F4_DURATION);
			}
			break;
		case 5:
			if(F5_DURATION < 99){
				F5_DURATION += 1;
				eeprom_write(0x05,F5_DURATION);
			}
			break;
		}
	}
}

void set_substract_duration_of_function(uint8_t _selected_menu){
	if(_selected_menu > 0){
		switch (_selected_menu) {
		case 1:
			if(F1_DURATION >0){
				F1_DURATION -= 1;
				eeprom_write(0x01,F1_DURATION);
			}
			break;
		case 2:
			if(F2_DURATION >0){
				F2_DURATION -= 1;
				eeprom_write(0x02,F2_DURATION);
			}
			break;
		case 3:
			if(F3_DURATION >0){
				F3_DURATION -= 1;
				eeprom_write(0x03,F3_DURATION);
			}
			break;
		case 4:
			if(F4_DURATION >0){
				F4_DURATION -= 1;
				eeprom_write(0x04,F4_DURATION);
			}
			break;
		case 5:
			if(F5_DURATION >0){
				F5_DURATION -= 1;
				eeprom_write(0x05,F5_DURATION);
			}
			break;
		}
	}
}
void add_coin_credit(uint32_t pulse_width) {
	max7219_DisableDisplayTest();
	if (pulse_width <= (coin_acceptor_pulse_width + creditPulseOffset)
			&& (pulse_width
					>= coin_acceptor_pulse_width - creditPulseOffset)) {
		char tmp_msg[40];
		sprintf(tmp_msg, "added credit : %d \r\n",
				(int)coin_credit_per_pulse);
		HAL_UART_Transmit(&huart1, (uint8_t*)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
		credit += coin_credit_per_pulse;
		//		eeprom_write(0x06,credit);
		if(credit >= 999){
			credit = 999;
		}
		segment_display_int(credit);

		if (credit >= minimum_credit_to_start) {
			system_function_start = true;
			HAL_UART_Transmit(&huart1, (uint8_t*)"front btn is now enable\r\n", 25,
					HAL_MAX_DELAY);
		}
	}else{
		char pulseWmessage[30];
		sprintf(pulseWmessage, "PULSEWIDTH NOT MATCH : %d \r\n", (int)pulse_width);
		HAL_UART_Transmit(&huart1, (uint8_t*)pulseWmessage, strlen(pulseWmessage), HAL_MAX_DELAY);
	}
}
void add_bank_note_credit(uint32_t pulse_width) {
	max7219_DisableDisplayTest();
	if (pulse_width <= bank_acceptor_pulse_width + creditPulseOffset
			&& pulse_width
			>= bank_acceptor_pulse_width - creditPulseOffset) {
		char tmp_msg[40];
		sprintf(tmp_msg, "added credit : %d \r\n",
				bank_credit_per_pulse);
		HAL_UART_Transmit(&huart1, (uint8_t*)tmp_msg, strlen(tmp_msg), HAL_MAX_DELAY);
		credit += bank_credit_per_pulse;
		//		eeprom_write(0x06,credit);
		if(credit >= 999){
			credit = 999;
		}
		segment_display_int(credit);
		if (credit >= minimum_credit_to_start) {
			system_function_start = true;
			HAL_UART_Transmit(&huart1, (uint8_t*)"front btn is now enable\r\n", 25,
					HAL_MAX_DELAY);
		}
		char tmp_msg2[35];
		sprintf(tmp_msg2, "current credit : %d \r\n", (int)credit);
		HAL_UART_Transmit(&huart1, (uint8_t*)tmp_msg2, strlen(tmp_msg2),
				HAL_MAX_DELAY);
	}else{
		char pulseWmessage[30];
		sprintf(pulseWmessage, "PULSEWIDTH NOT MATCH : %d \r\n", (int)pulse_width);
		HAL_UART_Transmit(&huart1, (uint8_t*)pulseWmessage, strlen(pulseWmessage), HAL_MAX_DELAY);
	}
}

void logic_runner() {
	eeprom_write(0x06,credit);
	max7219_DisableDisplayTest();
	if (selected_button != 0) {
		logic_runner_round_counter += 1;
	}
	switch(selected_button){
	case 3:
		duration_per_1credit = F1_DURATION;
		break;
	case 4:
		duration_per_1credit = F2_DURATION;
		break;
	case 5:
		duration_per_1credit = F3_DURATION;
		break;
	case 6:
		duration_per_1credit = F4_DURATION;
		break;
	case 7:
		duration_per_1credit = F5_DURATION;
		break;
	default:
		duration_per_1credit = 10;
		break;
	}
	if(duration_per_1credit != 0){
		if (logic_runner_round_counter >= duration_per_1credit && credit > 0) {
			logic_runner_round_counter = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)"took 1 credit\r\n", 15, HAL_MAX_DELAY);
			credit -= 1;
			if(credit < 255){
				//			eeprom_write(0x06,credit);
				HAL_UART_Transmit(&huart1, (uint8_t *)"writing credit value\r\n", 22,
						HAL_MAX_DELAY);
			}
			serial_display_credit();
		}
	}

	if (credit <= 0) {
		//		segment_display_standby();
		reset_all_output();
		HAL_UART_Transmit(&huart1, (uint8_t *)"time up !, disable all functions\r\n", 34,
				HAL_MAX_DELAY);
		credit = 0;
		//		eeprom_write(0x06,0);
		system_function_start = false;
		selected_button = 0;
	}else{
		segment_display_int(credit);
	}
}

void serial_display_credit() {
	char buffer[35];
	sprintf(buffer, "current credit : %d \r\n", (int)credit);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}
void stop_and_clear_tim1() {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	//	HAL_UART_Transmit(&huart2, message3, strlen(message3), HAL_MAX_DELAY);
	//	stop_and_clear_tim1();
	//	max7219_Turn_On();
	HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);


	if (htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) { // if the interrupt source is channel1
		HAL_UART_Transmit(&huart1, (uint8_t*)"CH1 INT\r\n", 9, HAL_MAX_DELAY);
		if (Is_First_Captured == 0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured == 1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
			if (IC_Val2 > IC_Val1) {
				Difference = IC_Val2 - IC_Val1;
			}

			else if (IC_Val1 > IC_Val2) {
				Difference = ((uint16_t) 0xffff - IC_Val1) + IC_Val2;
			}
			Is_First_Captured = 0; // set it back to false
			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_RISING);

			add_bank_note_credit(Difference/1000);
			last_credit_insert_bank += bank_credit_per_pulse;
			char tmpp[35];
			sprintf(tmpp,"CH1:captured val : %d \r\n",(int)Difference);
			HAL_UART_Transmit(&huart1, (uint8_t*)tmpp, strlen(tmpp), HAL_MAX_DELAY);
		}
	}
	if (htim->Instance == TIM3
			&& htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) { // if the interrupt source is channel1
		HAL_UART_Transmit(&huart1, (uint8_t*)"CH2 INT\r\n", 9, HAL_MAX_DELAY);
		if (coin_Is_First_Captured == 0) // if the first value is not captured
		{
			coin_IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // read the first value
			coin_Is_First_Captured = 1;  // set the first captured as true

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (coin_Is_First_Captured == 1) // if the first is already captured
		{
			coin_IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
			if (coin_IC_Val2 > coin_IC_Val1) {
				coin_Difference = coin_IC_Val2 - coin_IC_Val1;
			}

			else if (coin_IC_Val1 > coin_IC_Val2) {
				coin_Difference = ((uint16_t) 0xffff - coin_IC_Val1)
																																																																																																																						+ coin_IC_Val2;
			}
			coin_Is_First_Captured = 0; // set it back to false
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2,
					TIM_INPUTCHANNELPOLARITY_RISING);
			add_coin_credit(coin_Difference/1000);
			last_credit_insert += coin_credit_per_pulse;
			char tmpp[35];
			sprintf(tmpp,"CH2:captured val : %d \r\n",(int)coin_Difference);
			HAL_UART_Transmit(&huart1, (uint8_t*)tmpp, strlen(tmpp), HAL_MAX_DELAY);
		}
	}
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC2);
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC2);
	HAL_TIM_Base_Start_IT(&htim2);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
