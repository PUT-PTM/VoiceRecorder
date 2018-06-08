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
#include <stdio.h>
#include "fpu.h"
#include "spi_sd.h"
#include "ff.h"
#include <string.h>
#include <stdint.h>

FATFS fatfs;
FIL fileWAVE;
FRESULT SD_fres;
UINT sav_bytes = 0;;

//WAVE

int FileCounter = 0;  //Counter of files
uint32_t lengths = 8;	//Length of the file in seconds
void MakeFile();
extern uint8_t rawAudio[123200];





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
		    SD_fres = f_mount( 0, &fatfs );


		    GPIO_SetBits(GPIOD, GPIO_Pin_13);
		       	GPIO_SetBits(GPIOD, GPIO_Pin_14);
		       	GPIO_SetBits(GPIOD, GPIO_Pin_15);
		       	for(int i=0;i<3;i++)
		        {MakeFile();}

		       GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		       GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		       GPIO_ResetBits(GPIOD, GPIO_Pin_15);

	for(;;);
}





void MakeFile()

{
	//Set filename
	FileCounter++;
	char fileName[11];
	sprintf(fileName, "File%d.wav", FileCounter);

	uint32_t empty = 0; //Set empty integer for Sizes
	uint32_t ForDat = 16; //Set length of format data
	uint16_t Type = 1; //Set it as PCM file
	uint16_t Channels = 1; //1 for Mono, 2 for Stereo
	uint32_t SampleRate = 16000; //Number of Samples per sec
	uint16_t BitsPerSample = 8;
	uint32_t ByteRate = SampleRate * Channels * BitsPerSample / 8;  //Average rate of bytes
	uint16_t BlockAllign = Channels * BitsPerSample / 8;  //Bytes per sample including all channels
	uint32_t FileSize = SampleRate * lengths + 44 - 8;
	uint32_t DataSize = SampleRate * lengths;

	uint32_t NumOfSampleSets = SampleRate * lengths; //Get the number of Sample sets



	//init constants:
	char * RIFF = ("RIFF");
	char * WAVE = ("WAVE");
	char * fmt = ("fmt ");
	char * data = ("data");




    uint32_t z = 0;

	SD_fres = f_open( &fileWAVE, fileName, FA_OPEN_ALWAYS | FA_WRITE );
	if (1==1)
	{

		//HEADER
		SD_fres = f_write(&fileWAVE, ( const void * ) RIFF,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &empty,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, ( const void * ) WAVE,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, ( const void * ) fmt, 4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &ForDat,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &Type,2, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &Channels,2, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &SampleRate,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &ByteRate,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &BlockAllign,2, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &BitsPerSample,2, &sav_bytes);
		SD_fres = f_write(&fileWAVE, ( const void * ) data,4, &sav_bytes);
		SD_fres = f_write(&fileWAVE, &empty,4, &sav_bytes);
	}
	    z += 44;

	    //--------------------------------------------------------------------

		uint32_t c = NumOfSampleSets;
		//uint16_t emptysample = 100;
        uint32_t d = 0;
		while (c >= 1)
		{
		    if(d >= sizeof(rawAudio)/sizeof(rawAudio[0])) {d = 0; printf("%d", (int)d);}
		    uint8_t sample = rawAudio[d];
		    d++; z += 1;
		    SD_fres = f_write(&fileWAVE, &sample,1, &sav_bytes);
		    c--;
		}

//-------------------------------------
		FileSize = z -8;
		DataSize = z -44;

		f_lseek(&fileWAVE, 4);
	    SD_fres = f_write(&fileWAVE, &FileSize,4, &sav_bytes);
	    f_lseek(&fileWAVE, 44);
		SD_fres = f_write(&fileWAVE, &DataSize,4, &sav_bytes);


	//fclose(fs)
	SD_fres = f_close( &fileWAVE );

}















