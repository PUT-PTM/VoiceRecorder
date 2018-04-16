
/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include "main.h"
#include "pdm_filter.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx.h"

uint32_t InternalBufferSize = 0;
uint32_t Data_Status = 0;

/* Private function prototypes -----------------------------------------------*/
static void GPIO_Configure(void);
static void I2S_Configure(void);
static void NVIC_Configure(void);
static void RCC_Configure(void);



void SPI2_IRQHandler(void){
  extern PDMFilter_InitStruct Filter;
  extern uint8_t  PDM_Input_Buffer[];
  extern uint16_t PCM_Output_Buffer[];

  u16 volume;
  u16 app;

  // SprawdŸ czy s¹ dostêpne nowe dane
  if (SPI_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET){
    // Odczytaj dane i zapisz do bufora – najpierw m³odszy potem starszy bajt
    app = SPI_I2S_ReceiveData(SPI2);
    PDM_Input_Buffer[InternalBufferSize++] = (uint8_t)app;
    PDM_Input_Buffer[InternalBufferSize++] = (uint8_t)HTONS(app);

    // SprawdŸ czy bufor jest pe³ny
    if (InternalBufferSize >= PDM_Input_Buffer_SIZE){
      InternalBufferSize = 0;

      volume = 50;

      PDM_Filter_64_LSB(PDM_Input_Buffer, PCM_Output_Buffer, volume, &Filter);
      Data_Status = 1;
    }
  }
 }

/* Functions -----------------------------------------------------------------*/

int main(void)
{


	 extern uint32_t Data_Status;




	  RCC_Configure();
	  NVIC_Configure();
	  GPIO_Configure();
	  I2S_Configure();

	  // Initialize PDM filter
	  Filter.Fs = OUT_FREQ;
	  Filter.HP_HZ = 10;
	  Filter.LP_HZ = 16000;
	  Filter.In_MicChannels = 1;
	  Filter.Out_MicChannels = 1;
	  PDM_Filter_Init(&Filter);

	int32_t PDM_Filter_64_MSB(uint8_t* data, uint16_t* dataOut, uint16_t MicGain,  PDMFilter_InitStruct * Filter);
	int32_t PDM_Filter_80_MSB(uint8_t* data, uint16_t* dataOut, uint16_t MicGain,  PDMFilter_InitStruct * Filter);
	int32_t PDM_Filter_64_LSB(uint8_t* data, uint16_t* dataOut, uint16_t MicGain,  PDMFilter_InitStruct * Filter);
	int32_t PDM_Filter_80_LSB(uint8_t* data, uint16_t* dataOut, uint16_t MicGain,  PDMFilter_InitStruct * Filter);
}

/* Private functions ---------------------------------------------------------*/
static void GPIO_Configure(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure MP45DT02's CLK / I2S2_CLK (PB10) line
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Configure MP45DT02's DOUT / I2S2_DATA (PC3) line
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);  // Connect pin 10 of port B to the SPI2 peripheral
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);   // Connect pin 3 of port C to the SPI2 peripheral
}

static void I2S_Configure(void){
  I2S_InitTypeDef I2S_InitStructure;

  SPI_I2S_DeInit(SPI2);
  I2S_InitStructure.I2S_AudioFreq = OUT_FREQ*2;
  I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  I2S_Init(SPI2, &I2S_InitStructure);

  // Enable the Rx buffer not empty interrupt
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
}

static void NVIC_Configure(void){
  NVIC_InitTypeDef NVIC_InitStructure;

  // Configure the interrupt priority grouping
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  // Configure the SPI2 interrupt channel
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

static void RCC_Configure(void){
  // Enable CRC module - required by PDM Library
  //RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;

  /********/
  /* AHB1 */
  /********/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
                         RCC_AHB1Periph_CRC, ENABLE);

  /********/
  /* APB1 */
  /********/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  RCC_PLLI2SCmd(ENABLE);
}
