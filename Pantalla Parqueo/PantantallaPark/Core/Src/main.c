/* USER CODE BEGIN Header */
/*
 * Este código es la pantalla del videojuego proyecto 2 de electronica digital 2
 * Para este código se implementaron varios conceptos aprendidos en clase
 */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//Se incluyen todas las librerias que se van a utilizar.
#include "fatfs_sd.h"
#include "ili9341.h"
#include "bitmaps.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "neopixel.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t dma_libre = 1;
uint8_t cant_cars = 0; 
//uint8_t PosCars = 0;
uint8_t sot2 = 0;

uint8_t cant1 = 0;
uint8_t cant2 = 0;

char num;

uint16_t park[68*20];
uint16_t carA[246*68];
uint16_t he[183*80];

uint8_t park1A = 0;
uint8_t park2A = 0;
uint8_t park3A = 0;
uint8_t park4A = 0;
uint8_t park5A = 0;


uint8_t park1B = 0;
uint8_t park2B = 0;
uint8_t park3B = 0;
uint8_t park4B = 0;
uint8_t park5B = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc2;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_ch1;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
volatile uint16_t ADCVal[5];
volatile uint16_t sotB[4];
volatile uint8_t adc_index = 0;
float brilloled;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART5_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//********************************FUNCIONES VARIAS *********************************//
//-------------------Función para var cantidad de parqueos libres ----------------------

void parqueos_disponiblesA(uint8_t cant)
{
	if(cant == 0)
	{
		LCD_Print("4", 270, 140, 2, 0x053d, 0xe71c);
	}
	else if (cant == 1)
	{
		LCD_Print("3", 270, 140, 2, 0x053d, 0xe71c);
	}
	else if (cant == 2)
		{
		LCD_Print("2", 270, 140, 2, 0x053d, 0xe71c);
		}
	else if (cant == 3)
		{
		LCD_Print("1", 270, 140, 2, 0x053d, 0xe71c);
		}
	else if (cant == 4)
		{
		LCD_Print("0", 270, 140, 2, 0x053d, 0xe71c);
		}
	//Logica de envio de datos a la LCD o Display
}


void parqueos_disponiblesB(uint8_t cant)
{
	if (cant == 0)
		{
			LCD_Print("4", 270, 177, 2, 0x053d, 0xe71c);
		}
	else if (cant == 1)
		{
			LCD_Print("3",270, 177, 2, 0x053d, 0xe71c);
		}
	else if (cant == 2)
		{
			LCD_Print("2",270, 177, 2, 0x053d, 0xe71c);
		}
	else if (cant == 3)
		{
			LCD_Print("1",270, 177, 2, 0x053d, 0xe71c);
		}
	else if (cant == 4)
		{
			LCD_Print("0",270, 177, 2, 0x053d, 0xe71c);
		}
}

void borrar_sprite(int x, int y, int w, int h)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            uint16_t pixel = parking[(y + j) * 320 + (x + i)];
            park[j * w + i] = (pixel << 8) | (pixel >> 8);
        }
    }

    while(!dma_libre);
    dma_libre = 0;
    LCD_Bitmap_DMA(x, y, w, h, park);

}

void actualizar_leds(void)
{
    // Parqueos A
    for (int i = 0; i < 5; i++)
    {
        if (ADCVal[i] < 1600) // ocupado
        {
            setPixelColor(i, 255, 0, 0); // ROJO
        }
        else
        {
            setPixelColor(i, 0, 255, 0); // VERDE
        }
    }
    pixelShow();
}

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_FATFS_Init();
  MX_UART5_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(0x00);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ADCVal, 5);
	LCD_Bitmap(0, 0, 320, 240, parking);
	LCD_Print("0", 270, 140, 2, 0x053d, 0xe71c); // Numero inicial en A
	LCD_Print("0", 270, 177, 2, 0x053d, 0xe71c); // Numero inicial en B

	LCD_DibujarSpriteUniversal(260, 205, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);

	pixelClear();
	setBrightness(50); // o el valor que quieras
	pixelShow();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint32_t ultimo_print_uart = 0;

	char uart_buf[100];

	while (1) {
		//Logica para detección parqueo ocupado y Animació


	if (HAL_GetTick() - ultimo_print_uart >= 500) {
				ultimo_print_uart = HAL_GetTick();

				sprintf(uart_buf, "CH0: %u | CH1: %u | CH9: %u | CH10: %u\r\n",
						ADCVal[0], ADCVal[1], ADCVal[2], ADCVal[3]);

				HAL_UART_Transmit(&huart2, (uint8_t *)uart_buf, strlen(uart_buf), 100);
			}
	//-------------------------Parqueo A1 animación--------------------------
	if (ADCVal[0]<1600)
	{
		if (park1A == 0)
		{
			uint8_t val = rand() % 6;
			park1A = 1;
			LCD_DibujarSpriteUniversal(12, 10, 41, 68, carroA, val, 246, parking, 320, 0xa501, carA);
			LCD_DibujarSpriteUniversal(14, 90, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
			cant1++;
			parqueos_disponiblesA(cant1);
		}
	}

	else if (ADCVal[0]>1800)
	{
		if (park1A == 1)
		{
			park1A = 0;
			borrar_sprite(12, 10, 41, 68);
			LCD_DibujarSpriteUniversal(14, 90, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
			cant1--;
			parqueos_disponiblesA(cant1);
		}

	}




	//-------------------------Parqueo A2 animación--------------------------
	if (ADCVal[1]<1600)
	{
		if (park2A == 0)
		{
			park2A = 1;
			uint8_t val = rand() % 6;
			LCD_DibujarSpriteUniversal(60, 10, 41, 68, carroA, val, 246, parking, 320, 0xa501, carA);
			LCD_DibujarSpriteUniversal(65, 90, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
			cant1++;
			parqueos_disponiblesA(cant1);
		}
	}

	else if (ADCVal[1]>1600)
	{
		if (park2A == 1)
		{
			park2A = 0;
			borrar_sprite(60, 10, 41, 68);
			LCD_DibujarSpriteUniversal(65, 90, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
			cant1--;
			parqueos_disponiblesA(cant1);
		}
	}



	//-------------------------Parqueo A3 animación--------------------------
	if (ADCVal[2]<1500)
	{
		if (park3A == 0)
		{
			uint8_t val = rand() % 6;
			park3A = 1;
			LCD_DibujarSpriteUniversal(109, 10, 41, 68, carroA, val, 246, parking, 320, 0xa501, carA);
			LCD_DibujarSpriteUniversal(112, 90, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
			cant1++;
			parqueos_disponiblesA(cant1);
		}
	}

	else if (ADCVal[2]>1500)
	{
		if (park3A == 1)
		{
			park3A = 0;
			borrar_sprite(109, 10, 41, 68);
			LCD_DibujarSpriteUniversal(112, 90, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
			cant1--;
			parqueos_disponiblesA(cant1);
		}
	}





	//-------------------------Parqueo A4 animación--------------------------
	if (ADCVal[3]<1500)
	{
		if (park4A == 0)
		{
			uint8_t val = rand() % 6;
			park4A = 1;
			LCD_DibujarSpriteUniversal(156, 10, 41, 68, carroA, val, 246, parking, 320, 0xa501, carA);
			LCD_DibujarSpriteUniversal(160, 90, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
			cant1++;
			parqueos_disponiblesA(cant1);
		}
	}

	else if (ADCVal[3]>1500)
	{
		if (park4A == 1)
		{
			park4A = 0;
			borrar_sprite(156, 10, 41, 68);
			LCD_DibujarSpriteUniversal(160, 90, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
			cant1--;
			parqueos_disponiblesA(cant1);
		}
	}



	//-------------------------Parqueo Helipuerto animación--------------------------
	if (ADCVal[4]<1500)
		{
			if (park5A == 0)
			{
				park5A = 1;
				LCD_DibujarSpriteUniversal(232, 10, 61, 80, helicop, 1, 183, parking, 320, 0xa501, he);
				LCD_DibujarSpriteUniversal(260, 205, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
			}

			//parqueos_disponiblesA(cant1);
		}

	else if (ADCVal[4]>1500)
	{
		if (park5A == 1)
		{
			park5A = 0;
			borrar_sprite(232, 10, 61, 80);
			LCD_DibujarSpriteUniversal(260, 205, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
		}

	}






	//-------------------------Parqueo B1 animación--------------------------
	if (sotB[0] == 1)
		{
			if (park1B == 0)
			{
				uint8_t val = rand() % 6;
				park1B = 1;
				LCD_DibujarSpriteUniversal(9, 163, 41, 68, carroB, val, 246, parking, 320, 0xa501, carA);
				LCD_DibujarSpriteUniversal(12, 130, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
				cant2++;
				parqueos_disponiblesB(cant2);
			}
		}

		else if (sotB[0] == 0)
		{
			if (park1B == 1)
			{
				park1B = 0;
				borrar_sprite(9, 163, 41, 68);
				//LCD_DibujarSpriteUniversal(60, 10, 41, 68, carroB, 2, 246, parking, 320, 0x8410, carB);
				LCD_DibujarSpriteUniversal(12, 130, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
				cant2--;
				parqueos_disponiblesB(cant2);
			}
		}




	//-------------------------Parqueo B2 animación--------------------------
		if (sotB[1] == 1)
		{
			if (park2B == 0)
			{
				park2B = 1;
				uint8_t val = rand() % 6;
				LCD_DibujarSpriteUniversal(58, 163, 41, 68, carroB, val, 246, parking, 320, 0xa501, carA);
				LCD_DibujarSpriteUniversal(63, 130, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
				cant2++;
				parqueos_disponiblesB(cant2);
			}
		}

		else if (sotB[1] == 0)
		{
			if (park2B == 1)
			{
				park2B = 0;
				borrar_sprite(58, 163, 41, 68);
				LCD_DibujarSpriteUniversal(63, 130, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
				cant2--;
				parqueos_disponiblesB(cant2);
			}
		}



	//-------------------------Parqueo B3 animación--------------------------
		if (sotB[2] == 1)
		{
			if (park3B == 0)
			{
				uint8_t val = rand() % 6;
				park3B = 1;
				LCD_DibujarSpriteUniversal(107, 163, 41, 68, carroB, val, 246, parking, 320, 0xa501, carA);
				LCD_DibujarSpriteUniversal(110, 130, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
				cant2++;
				parqueos_disponiblesB(cant2);
			}
		}


		else if (sotB[2] == 0)
		{
			if (park3B == 1)
			{
				park3B = 0;
				borrar_sprite(107, 163, 41, 68);
				LCD_DibujarSpriteUniversal(110, 130, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
				cant2--;
				parqueos_disponiblesB(cant2);
			}
		}


		//-------------------------Parqueo B4 animación--------------------------
		if (sotB[3] == 1)
		{
			if (park4B == 0)
			{
				uint8_t val = rand() % 6;
				park4B = 1;
				LCD_DibujarSpriteUniversal(155, 163, 41, 68, carroB, val, 246, parking, 320, 0xa501, carA);
				LCD_DibujarSpriteUniversal(158, 130, 34, 20, semaforo, 0, 68, parking, 320, 0xe71c, park);
				cant2++;
				parqueos_disponiblesB(cant2);
			}
		}

		else if (sotB[3] == 0)
		{
			if (park4B == 1)
			{
				park4B = 0;
				borrar_sprite(155, 163, 41, 68);
				LCD_DibujarSpriteUniversal(158, 130, 34, 20, semaforo, 1, 68, parking, 320, 0xe71c, park);
				cant2--;
				parqueos_disponiblesB(cant2);
			}
		}

		actualizar_leds();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 5;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 105-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin SD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        dma_libre = 1;
    }
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
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
