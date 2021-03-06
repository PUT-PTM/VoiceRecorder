# VoiceRecorder 

## Overview:

Voice recorded by extern microphone is then saved on SD card as WAVE file.

## How to use:

Insert SD card in the SD card reader, power up the device. To start recording press the button. When the LED becomes red, you can start speaking. If you want to  stop recording press the button second time. When the LED changes color to green, it means process have been finished, file is created.

## Hardware:
   | STM32F4 DISCOVERY | SD Card Reader | External Microphone | RGB LED | 
   | --- | --- | --- | --- |
   
   You will also need SD card on which you'll save the files.
   
## How to run:
   | **STM32F4 DISCOVERY** | **SD Card Reader** |
   | --- | --- |
   | 3V | VCC |
   | GND | GND |
   | PB13 | SCK | 
   | PB14 | MISO | 
   | PB15 | MOSI | 
   | PB11 | CS | 
   
   | **STM32F4 DISCOVERY** | **External Microphone** |
   | --- | --- |
   | 3V | VCC |
   | GND | GND |
   | PA1 | AOUT | 
   
   | **STM32F4 DISCOVERY** | **RGB LED** |
   | --- | --- |
   | GND | GND |
   | PB7 | R |
   | PB8 | G |
   | PB5 | B |
 
  
## External libraries used:
  | **FatFs** by ElmChan |
  | --- |
  
## Credits:

[Tomasz Jóskowiak](https://github.com/TJoskowiak), [Adam Klejda](https://github.com/AdamKlejda)

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.

Supervisor: Tomasz Mańkowski
