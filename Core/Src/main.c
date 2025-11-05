/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "dma.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keyled.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t SDBuf_TX[BLOCKSIZE];	//Data Sending Cache, BLOCKSIZE=512
uint8_t SDBuf_RX[BLOCKSIZE];	//Data Received Cache
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  MX_SDIO_SD_Init();
  /* USER CODE BEGIN 2 */
  uint8_t startstr[] = "Demo13_2: SD card R/W-DMA. \r\n";
  HAL_UART_Transmit(&huart3,startstr,sizeof(startstr),0xFFFF);

  uint8_t startstr1[] = "Read/write SD card via DMA. \r\n\r\n";
  HAL_UART_Transmit(&huart3,startstr1,sizeof(startstr1),0xFFFF);

  //show menu
  printf("[S2]KeyUp   =SD card info. \r\n");
  printf("[S3]KeyDown =Erase 0-10 blocks. \r\n");
  printf("[S4]KeyLeft =Write block. \r\n");
  printf("[S1]KeyRight=Read block. \r\n\r\n");

  while(1)
  {
	  KEYS waitKey=ScanPressedKey(KEY_WAIT_ALWAYS);	//Waiting for the key

	  if (waitKey==KEY_UP)
		{
			SDCard_ShowInfo();
			printf("Reselect menu item or reset. \r\n");
		}
		else if (waitKey== KEY_DOWN)
		{
			SDCard_EraseBlocks();			//EraseBlocks 0-10
			printf("Reselect menu item or reset. \r\n");
		}
		else if (waitKey== KEY_LEFT)
			SDCard_TestWrite_DMA();
		else if (waitKey== KEY_RIGHT)
			SDCard_TestRead_DMA();

		HAL_Delay(500);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* HAL_SD_GetCardInfo(), Display SD card information */
void SDCard_ShowInfo()
{
	HAL_SD_CardInfoTypeDef cardInfo;  //SD card information structure
	HAL_StatusTypeDef res=HAL_SD_GetCardInfo(&hsd, &cardInfo);

	if (res!=HAL_OK)
	{
		printf("HAL_SD_GetCardInfo() error. \r\n");
		return;
	}

	printf("*** HAL_SD_GetCardInfo() info *** \r\n\r\n");

	printf("Card Type= %ld \r\n", cardInfo.CardType);
	printf("Card Version= %ld \r\n", cardInfo.CardVersion);
	printf("Card Class= %ld \r\n", cardInfo.Class);
	printf("Relative Card Address= %ld \r\n", cardInfo.RelCardAdd);
	printf("Block Count= %ld \r\n", cardInfo.BlockNbr);
	printf("Block Size(Bytes)= %ld \r\n", cardInfo.BlockSize);
	printf("LogiBlockCount= %ld \r\n", cardInfo.LogBlockNbr);
	printf("LogiBlockSize(Bytes)= %ld \r\n", cardInfo.LogBlockSize);

	uint32_t cap = cardInfo.BlockNbr/1024;	//KB
	cap	= cap*cardInfo.BlockSize;			//KB
	cap = cap/1024;							//MB

	printf("SD Card Capacity(MB)= %ld \r\n\r\n", cap);
}

/* HAL_SD_Erase(), Erase SD card Blocks*/
void SDCard_EraseBlocks()
{
	uint32_t BlockAddrStart=0; 	// Block 0��Addresses using block number
	uint32_t BlockAddrEnd=10; 	// Block 10

	printf("\r\n*** Erasing blocks *** \r\n\r\n");

	if (HAL_SD_Erase(&hsd,BlockAddrStart, BlockAddrEnd)==HAL_OK)
	{
		printf("Erasing blocks,OK. \r\n");
		printf("Blocks 0-10 is erased. \r\n");
	}
	else
		printf("Erasing blocks,fail. \r\n");

	HAL_SD_CardStateTypeDef cardState=HAL_SD_GetCardState(&hsd);
	printf("GetCardState()= %ld \r\n", cardState);

	// The following code has nothing to do with erasure and can be deleted.[wen]
	while(cardState != HAL_SD_CARD_TRANSFER)   //Wait for return to transmission status
	{
		HAL_Delay(1);
		cardState=HAL_SD_GetCardState(&hsd);
		return;			//Otherwise, it will not come out after entering the loop
	}
}

/* HAL_SD_WriteBlocks_DMA(), Write SD card */
void SDCard_TestWrite_DMA()	//DMA mode, test write
{
	printf("\r\n*** DMA Writing blocks *** \r\n\r\n");

	for(uint16_t i=0;i<BLOCKSIZE; i++)
		SDBuf_TX[i]=i; 						//generate data

	printf("Writing block 6. \r\n");
	printf("Data in [10:15] is: %d ",SDBuf_TX[10]);

	for (uint16_t j=11; j<=15;j++)
	{
		printf(", %d", SDBuf_TX[j]);
	}
	printf("\r\nHAL_SD_WriteBlocks_DMA() is to be called. \r\n");

	uint32_t BlockAddr=6;		//Block Address
	uint32_t BlockCount=1;		//Block Count
	HAL_SD_WriteBlocks_DMA(&hsd,SDBuf_TX,BlockAddr,BlockCount);  //can erase block automatically
}

/* HAL_SD_ReadBlocks_DMA(), Read SD card */
void SDCard_TestRead_DMA()  //test read
{
	printf("\r\n*** DMA Reading blocks *** \r\n\r\n");
	printf("\r\nHAL_SD_ReadBlocks_DMA() is to be called. \r\n");

	uint32_t BlockAddr=6;		//Block Address
	uint32_t BlockCount=1;		//Block Count
	HAL_SD_ReadBlocks_DMA(&hsd,SDBuf_RX,BlockAddr,BlockCount);
}

/* SD write callback func */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
	printf("DMA write complete. \r\n");
	printf("HAL_SD_TxCpltCallback() is called. \r\n");
	printf("Reselect menu item or reset. \r\n");
}

/* SD read callback func */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
	printf("DMA read complete. \r\n");
	printf("HAL_SD_RxCpltCallback() is called. \r\n");
	printf("Data in [10:15] is: %d\r\n",SDBuf_RX[10]);

	for (uint16_t j=11; j<=15;j++)
	{
		printf(", %d", SDBuf_RX[j]);
	}
	printf("\r\nReselect menu item or reset. \r\n");
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
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
  while (1)
  {
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
