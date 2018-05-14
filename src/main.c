/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "delay.h"
#include "fpu.h"
#include "spi_sd.h"
#include "ff.h"

FATFS fatfs;
FIL plik;
FRESULT fresult;
UINT zapisanych_bajtow = 0;
UINT zapianie_bajtow = 0;
char * buffor = "123456789abcdef\r\n";
uint16_t i;

int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


		/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		SystemInit();
		    fpu_enable();
		    delay_init( 168 );
		    SPI_SD_Init();
		    GPIO_SetBits(GPIOD, GPIO_Pin_12);
		    fresult = f_mount( 0, &fatfs );


		    GPIO_SetBits(GPIOD, GPIO_Pin_13);
		    fresult = f_open( &plik, "plik.txt", FA_OPEN_ALWAYS | FA_WRITE );
		       if( fresult == FR_OK )
		       {

		       	GPIO_SetBits(GPIOD, GPIO_Pin_14);
		       	GPIO_SetBits(GPIOD, GPIO_Pin_15);

		       	for( i = 0; i < 1000; i++ )
		               fresult = f_write( &plik, ( const void * ) buffor, 17, &zapisanych_bajtow );
		       }

		       fresult = f_close( &plik );

	for(;;);
}
