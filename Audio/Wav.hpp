#pragma once
#ifndef AUDIO_WAV_HPP
#define AUDIO_WAV_HPP

#include <fstream>

/*
#include <Audio/Wav.hpp>
Class to load wav audio files. Some code copied from: http://www.codeguru.com/forum/archive/index.php/t-460547.html
This library seems to have trouble with metadata. Stripping the metadata using Audacity can fix this problem, though I'd like to fix this bug later.
*/
class Wav
{
	public:
	
	double duration;
	
	int fileSize;
	int nAudioBytes;
	
	char* data;
	
	char chunkID[4]; /* Must be 'RIFF'. */
	char format[4]; /* Must be 'WAVE'. */
	
	
		char Subchunk1ID[4],Subchunk2ID[4];
		int ChunkSize,Subchunk1Size, SampleRate, ByteRate,Subchunk2Size;
		short AudioFormat, NumChannels, BlockAlign, BitsPerSample;
	
	
	Wav()
	{}
	

	// void readFileVerbose(const char* PATH)
	// {
	
	// }
	
	void readFile(const char* PATH)
	{
		// Read the wave file
		FILE *fhandle=fopen(PATH,"rb");
		
		if (!fhandle)
		{
			std::cout<<"ERROR: Wav file couldn't be opened.\n";
			return;
		}
		
		fread(chunkID,1,4,fhandle);
		
		if(chunkID[0]=='R'&&chunkID[1]=='I'&&chunkID[2]=='F'&&chunkID[3]=='F')
		{
			fread(&fileSize,4,1,fhandle);
			fread(format,1,4,fhandle);
		
         if(format[0]=='W'&&format[1]=='A'&&format[2]=='V'&&format[3]=='E')
         {
            fread(Subchunk1ID,1,4,fhandle);
            fread(&Subchunk1Size,4,1,fhandle);
            fread(&AudioFormat,2,1,fhandle);
            fread(&NumChannels,2,1,fhandle);
            fread(&SampleRate,4,1,fhandle);
            fread(&ByteRate,4,1,fhandle);
            fread(&BlockAlign,2,1,fhandle);
            fread(&BitsPerSample,2,1,fhandle);
            fread(&Subchunk2ID,1,4,fhandle);
            fread(&Subchunk2Size,4,1,fhandle);
            nAudioBytes=Subchunk2Size/(BitsPerSample/8)*NumChannels;
            data=new char [nAudioBytes]; // Create an element for every sample
            fread(data,1,nAudioBytes,fhandle); // Reading raw audio data byte by byte.
            fclose(fhandle);
            
            std::cout<<"fileSize: "<<fileSize<<".\n";
            std::cout<<"nAudioBytes: "<<nAudioBytes<<".\n";

            /* Exact audio length is ChunkSize/ByteRate. */
            duration = (double)nAudioBytes/ByteRate;
            std::cout<<"Duration: "<<duration<<".\n";

         
         }
         else
         {
            std::cout<<"Wave header not found.\n";
         }
		}
		else
		{
			std::cout<<"Wav.hpp Error: chunkID is invalid.\n";
		}
	}
	
	/* Convert the current data in the Wav object to a Sound object. */
	Sound* toSound()
	{
		Sound * sound = new Sound;
	
		sound->durationSeconds = duration;
		sound->nChannels=NumChannels;
		sound->bitsPerSample=BitsPerSample;
		sound->byteRate=ByteRate;
		sound->samplesPerSecond=SampleRate;
		sound->loadRawData(data,nAudioBytes);
		// else if (bitsPerSample==16)
		// {
			// sound->loadRawData16(data,nData2);
		// }
		// else
		// {
			// std::cout<<"Wav.hpp error: Bad bit per sample.\n";
		// }
		
	
	
		return sound;
	}
	
};




#endif /* #ifndef AUDIO_WAV_HPP */
