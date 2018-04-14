// WAVEfile_head.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdint.h>
#include <stdio.h>

int FileCounter = 0;  //Counter of files
int lengths = 8;	//Length of the file in seconds
void MakeFile();
extern uint8_t rawAudio[123200];
int main()
{
	int x = 0;
	while (x < 1)
		{
		MakeFile();
		x++;
		}
    return 0;
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


	uint32_t NumOfSampleSets = SampleRate * lengths; //Get the number of Sample sets



	//Open stream
	FILE *fs;
	fs = fopen(fileName, "w");
	if (fs != NULL)
	{

		//HEADER
		fwrite("RIFF", strlen("RIFF"), 1, fs);   //Marks as RIFF file										0
		printf("Size of Header %d", ftell(fs));
		fwrite(&empty, sizeof(uint32_t), 1, fs);   //Set 0 as intital filesize								4
		fwrite("WAVE", strlen("WAVE"), 1, fs); //Marks WAVE header											8
		fwrite("fmt ", strlen("fmt "), 1, fs); //fmt subchunk marker											12
		fwrite(&ForDat, sizeof(uint32_t), 1, fs);   //Set length of format data								16
		fwrite(&Type, sizeof(uint16_t), 1, fs);   //Set Type as PCM											20
		fwrite(&Channels, sizeof(uint16_t), 1, fs);   //Mono or Stereo? That is the question.				22
		fwrite(&SampleRate, sizeof(uint32_t), 1, fs);  // Number of Samples per Sec							24
		fwrite(&ByteRate, sizeof(uint32_t), 1, fs);   //Magical thing										28
		fwrite(&BlockAllign, sizeof(uint16_t), 1, fs);  // Bytes per Sample Ultimate Edition				32
		fwrite(&BitsPerSample, sizeof(uint16_t), 1, fs); //Bits per sample Normal Edition					34
		fwrite("data", strlen("data"), 1, fs);    //data subchunk marker									36
		fwrite(&empty, sizeof(uint32_t), 1, fs);   //Set 0 as intital datasize								40

		printf("Size of Head %d", BlockAllign);
		//DATA
		//Starts at 44
		//For testing only; Empty file.
		uint32_t c = NumOfSampleSets;
		uint16_t emptysample = 100;
        uint32_t d = 0;
		while (c >= 1)
		{
		    if(d >= sizeof(rawAudio)/sizeof(rawAudio[0])) {d = 0; printf("%d", d);}
		    uint8_t sample = rawAudio[d];
		    d++;
			fwrite(&sample, sizeof(uint8_t), 1, fs); //writing empty samples to file
			c--;
		}

		fseek(fs, 0, SEEK_END);
		uint32_t FileSize = ftell(fs) - 8;
		uint32_t DataSize = ftell(fs) - 44;
		fseek(fs, 4, SEEK_SET);
		fwrite(&FileSize, sizeof(uint32_t), 1, fs);   //Set filesize
		fseek(fs, 40, SEEK_SET);
		fwrite(&DataSize, sizeof(uint32_t), 1, fs);   //Set datasize


	}
	fclose(fs);


}
