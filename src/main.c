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
uint32_t z = 0;
int FileCounter = 0;  //Counter of files
//uint32_t lengths = 8;	//Length of the file in seconds

extern uint8_t rawAudio[123200];

uint32_t empty = 0; //Set empty integer for Sizes
uint32_t ForDat = 16; //Set length of format data
uint16_t Type = 1; //Set it as PCM file
uint16_t Channels = 1; //1 for Mono, 2 for Stereo
uint32_t SampleRate = 16000; //Number of Samples per sec
uint16_t BitsPerSample = 8;
uint32_t ByteRate = 16000; //SampleRate * Channels * BitsPerSample / 8;  //Average rate of bytes
uint16_t BlockAllign = 1;//Channels * BitsPerSample / 8;  //Bytes per sample including all channels
uint32_t FileSize = 0;//SampleRate * lengths + 44 - 8;
uint32_t DataSize =0; //SampleRate * lengths;




//adc
uint16_t ADC_Result =0;
void initADC();

void initRGB();
void RGB_Red();
void RGB_Green();
void RGB_Blue();
//Holders
void initTimH();
int c=0 ;

//----------------------
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		c++;
		//TIM_Cmd(TIM3, ENABLE);

	}
}
/*
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update)) {
			if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0)){

				c++; //////////////////////<---------co sie robi po wcisniecu przycisku
				TIM_Cmd(TIM3, DISABLE);
				TIM_SetCounter(TIM3, 0);
			}
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		}}}

*/


void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update)) {
			if(c==0){
			RGB_Blue();
		}
		if(c==1){
			//Set filename
			FileCounter++;
			char fileName[11];
			sprintf(fileName, "File%d.wav", FileCounter);


			//init constants:
			char * RIFF = ("RIFF");
			char * WAVE = ("WAVE");
			char * fmt = ("fmt ");
			char * data = ("data");

			 z = 0;
			 SD_fres = f_open( &fileWAVE, fileName, FA_OPEN_ALWAYS | FA_WRITE );

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

			 z += 44;

			 c++;
		}
		if(c==2){
			RGB_Red();

			ADC_SoftwareStartConv(ADC1);
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
			ADC_Result = ADC_GetConversionValue(ADC1);
			uint8_t sample =ADC_Result ;
			z += 1;
			SD_fres = f_write(&fileWAVE, &sample,1, &sav_bytes);


		}
		if(c==3){

			FileSize = z -8;
			DataSize = z -44;

			f_lseek(&fileWAVE, 4);
			SD_fres = f_write(&fileWAVE, &FileSize,4, &sav_bytes);
			f_lseek(&fileWAVE, 44);
			SD_fres = f_write(&fileWAVE, &DataSize,4, &sav_bytes);

			RGB_Green();



			SD_fres = f_close( &fileWAVE );




		}
		if(c==4){

			c=0;
		}




			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		}}}


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
		initADC();
		initRGB();
		initTimH();
		    fpu_enable();
		    delay_init( 168 );
		    SPI_SD_Init();
		    SD_fres = f_mount( 0, &fatfs );




	for(;;);
}



void initRGB(){

				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
				GPIO_InitTypeDef GPIO_InitStructure;
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
				GPIO_Init(GPIOB, &GPIO_InitStructure);

}
void RGB_Red()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_5);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

void RGB_Green()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_7 | GPIO_Pin_5);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

void RGB_Blue()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_7 | GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

void initTimH(){
	NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);




		EXTI_InitTypeDef EXTI_InitStructure;

		EXTI_InitStructure.EXTI_Line = GPIO_Pin_0;

		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

		EXTI_InitStructure.EXTI_LineCmd = ENABLE;

		EXTI_Init(&EXTI_InitStructure);

		SYSCFG_EXTILineConfig(GPIOA, EXTI_PinSource0);


		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //////////Timer usuwaacy drgania stykow
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Period = 10-1;
		TIM_TimeBaseStructure.TIM_Prescaler = (100-1);;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		NVIC_InitTypeDef NVIC_InitStructure1;
		// numer przerwania
		NVIC_InitStructure1.NVIC_IRQChannel = TIM3_IRQn;
		// priorytet g³ówny
		NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority =0;
		// subpriorytet
		NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 0;
		// uruchom dany kana³
		NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
		// zapisz wype³nion¹ strukturê do rejestrów
		NVIC_Init(&NVIC_InitStructure1);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM3, DISABLE);




		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);        //////////timer 16000hz
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure4;
		TIM_TimeBaseStructure4.TIM_Period = (10-1);
		TIM_TimeBaseStructure4.TIM_Prescaler = (525-1);
		TIM_TimeBaseStructure4.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure4.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure4);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		NVIC_InitTypeDef NVIC_InitStructure4;
		// numer przerwania
		NVIC_InitStructure4.NVIC_IRQChannel = TIM4_IRQn;
		// priorytet g³ówny
		NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority =10;
		// subpriorytet
		NVIC_InitStructure4.NVIC_IRQChannelSubPriority =10;
		// uruchom dany kana³
		NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE;
		// zapisz wype³nion¹ strukturê do rejestrów
		NVIC_Init(&NVIC_InitStructure4);

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM4, ENABLE);
}

void initADC(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // zegar dla portu GPIO z którego wykorzystany zostanie pin jako wejœcie ADC (PA1)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // zegar dla modu³u ADC1

	GPIO_InitTypeDef GPIO_InitStructure;
	//inicjalizacja wejœcia ADC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	// niezale¿ny tryb pracy przetworników
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	// zegar g³ówny podzielony przez 2
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	// opcja istotna tylko dla trybu multi ADC
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	// czas przerwy pomiêdzy kolejnymi konwersjami
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitTypeDef ADC_InitStructure;
	//ustawienie rozdzielczoœci przetwornika na maksymaln¹ (12 bitów)
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
	//wy³¹czenie trybu skanowania (odczytywaæ bêdziemy jedno wejœcie ADC
	//w trybie skanowania automatycznie wykonywana jest konwersja na wielu //wejœciach/kana³ach)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	//w³¹czenie ci¹g³ego trybu pracy
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	//wy³¹czenie zewnêtrznego wyzwalania
	//konwersja mo¿e byæ wyzwalana timerem, stanem wejœcia itd. (szczegó³y w //dokumentacji)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;

	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//wartoœæ binarna wyniku bêdzie podawana z wyrównaniem do prawej
	//funkcja do odczytu stanu przetwornika ADC zwraca wartoœæ 16-bitow¹
	//dla przyk³adu, wartoœæ 0xFF wyrównana w prawo to 0x00FF, w lewo 0x0FF0
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//liczba konwersji równa 1, bo 1 kana³
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	// zapisz wype³nion¹ strukturê do rejestrów przetwornika numer 1
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	ADC_Cmd(ADC1, ENABLE);

}
