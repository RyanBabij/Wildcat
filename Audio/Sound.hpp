#pragma once
#ifndef AUDIO_SOUND_HPP
#define AUDIO_SOUND_HPP

/*
#include <Audio/Sound.hpp>
Class to store sound data in internally portable format. Sound objects may be passed to audio players.
*/

#include <Math/Random/GlobalRandom.hpp>

#include <Data/DataTools.hpp>


class Sound
{
	public:
	
	int nChannels; /* How many audio tracks there are... Typically 1 for mono and 2 for stereo, though it can be more complicated than that. */
	int bitsPerSample; /* How many bits there are to store one piece of audio data. For this class only 8 and 16 are valid. */
	double durationSeconds; /* How long the audio clip lasts for. */
	//short int * data16; /* The raw audio data for 16 bit audio */
	char * data; /* The raw audio data. */
	int nData; /* The size of the data array. Not to be confused with the number of bytes of data. */
	int byteRate; /* How many bytes of data are contained for a second of audio. */
	int samplesPerSecond; /* Samples per second for each channel. Also known as hertz. */
	
	Random random;
	
	class CParamSmooth
	{
	public:
		CParamSmooth() { a = 0.9f; b = 1.f - a; z = 0; };
		//~CParamSmooth();
		inline int Process(int in) { z = (in * b) + (z * a); return z; }
	private:
		float a, b;
		int z;
	};
	
	void muteLeft()
	{
		int bytesPerSample=bitsPerSample/8;
		if(nChannels==2)
		{
			for ( int i=0;i<nData;i+=bytesPerSample*nChannels)
			{
				data[i]=0;
				data[i+1]=0;
			}
		}
		else
		{
			std::cout<<"Mute left only works for 2 channel audio.\n";
		}
	}
	
	// void smooth (int nNeighbors)
	// {
	
		// /*Left channel*/
	
		// j=0;

		// for (int i=0;i<nData;i+=bytesPerSample*nChannels)
		// {
		
			
			
			// if(j>nNeighbors+1 && j<nData-nNeighbors-1)
			// {
				// int nNeighbors2 = -nNeighbors;
				// int total=0;
				// for(;nNeighbors2<nNeighbors;++nNeighbors2)
				// {
					// //total+=data[
				// }
			
			// }
		
		// }
	// }
		
	/* Filters out high frequencies, leaving the bassy sounds. */
	void lowPassFilter()
	{
	
		//int someNumber=0;
		// std::string binary="";
		// for (double i=0;i<100;++i)
		// {
			// binary=DataTools::getBinaryString(i," ");
			// std::cout<<"Binary of "<<i<<": "<<binary<<".\n";
		// }
	
		// std::string binary = DataTools::getBinaryString(0);
		// std::cout<<"Binary of 0: "<<binary<<"\n";
		// binary = DataTools::getBinaryString(1);
		// std::cout<<"Binary of 1: "<<binary<<"\n";
		//char data2[nData];
		//std::cout<<"bitsPerSample: "<<bitsPerSample<<".\n";
		std::cout<<"Sizeof short int: "<<sizeof(short int)<<".\n";
		int bytesPerSample = bitsPerSample/8;
		CParamSmooth smoother;
		
		if(bytesPerSample==2 && nChannels==2)
		{
			std::cout<<"Good config.\n";
			
			/* Move the old data into a new array. */
			short int oldData[nData/nChannels];
			int nOldData=nData/nChannels;
			int j=0;
			for (int i=0;i<nData;i+=bytesPerSample*nChannels)
			{
			
				short int* sample = (short int*) &data[i];
				
				oldData[j++]=*sample;
				//std::cout<<"Sample: "<<*sample<<".\n";
				
				//if(j==100) { break; }
			}
			
			
			/* NEW: Move the old data into a vector. */
			
			Vector <short int> vOldData;
			
			for (int i=0;i<nData;i+=bytesPerSample*nChannels)
			{
				short int* sample = (short int*) &data[i];
				vOldData.push(*sample);
			}
			
			/* Do a sliding average on the vector, as a basic low pass filter. */
			vOldData.slidingAverage(64);
			
			std::cout<<"Overwriting datas.\n";
			int j2=0;
			/* Overwrite the sound data with the values in the vector. */
			for (int i=0;i<nData;i+=bytesPerSample*nChannels)
			{
			
				short int finalData = vOldData(j2);
				
				
								 short int* pData = ( short int*)&data[i];
								*pData&=0x0000;
								*pData|=finalData;
			
			//data[i] = vOldData(j);
			
				++j2;
			}
			std::cout<<"End overwriting datas.\n";
			
			
			
			
			/* Average out the surrounding samples (currently for left channel). Basically we're smoothing out the sound wave. */
			j=0;
			for (int i=0;i<nData;i+=bytesPerSample*nChannels)
			{
			
				/* Average the neighboring samples. */
				/* For now I'll leave potential overflow positions alone. */
				if(j>30 && j<nOldData-30)
				{
				
					int avg = oldData[j];
					avg+=oldData[j+1];
					avg+=oldData[j+2];
					avg+=oldData[j+3];
					avg+=oldData[j+4];
					avg+=oldData[j+5];
					avg+=oldData[j+6];
					avg+=oldData[j+7];
					avg+=oldData[j+8];
					avg+=oldData[j+9];
					avg+=oldData[j+10];
					avg+=oldData[j-1];
					avg+=oldData[j-2];
					avg+=oldData[j-3];
					avg+=oldData[j-4];
					avg+=oldData[j-5];
					avg+=oldData[j-6];
					avg+=oldData[j-7];
					avg+=oldData[j-8];
					avg+=oldData[j-9];
					avg+=oldData[j-10];
					//std::cout<<"Avg total: "<<avg<<".\n";
					avg/=20;
					//std::cout<<"final avg: "<<avg<<".\n";
				
					//unsigned int avg = ((unsigned int)oldData[j]+oldData[j+1]+oldData[j+2]+oldData[j-1]+oldData[j-2])/5;
					
				short int finalData = (short int)avg;
					//std::cout<<"Smoothed to: "<<finalData<<".\n";
					/* Overwrite the data with the smoothed value. */
					
								// short int* pData = ( short int*)&data[i];
								//*pData&=0x0000;
								//*pData|=finalData;
					

					
				}
				
				
				++j;
				
				//if(j==100) { break; }
			
			// std::cout<<"***\n";
				// short int sample = DataTools::valFromCharArray <short int> (&data[i]);
				// std::cout<<"Sample: "<<sample<<"\n";
				// std::string binary = DataTools::getBinaryString <short int> (sample);
				// std::cout<<"Binary: "<<binary<<".\n";
				
				
				//std::string binary2 = DataTools::getBinaryString <char> (data[i]);
				//std::string binary3 = DataTools::getBinaryString <char> (data[i+1]);
				//std::cout<<"Char binary values: "<<binary2<<" and "<<binary3<<".\n";
				
				//short int sample2 = DataTools::getBytesValue <short int> (&data[i],2);
				//char sample2 = 0;
				////sample2|=data[i];
				//char sample3=0;
				//sample3|=data[i+1];
				//sample2|=data[i+1];
				
				//unsigned int sample4=8;
				//sample4=sample4|sample2;
			// sample4>>=32;
			// sample4|=15;
			// sample4<<=8;
			// sample4|=7;
			// sample4<<=8;
			// sample4|=3;
			// sample4<<=8;
			// sample4|=1;
			//	sample4>>=8;
			//sample4|=0;
			//sample4>>=8;
			///sample4|=0;
			//sample4<<=16;
			
			// short int* sample = ( short int*) &data[i];
			//std::cout<<*sample<<".\n";
			//*sample=0;
			
			
			/* Convert audio data from char array into 2 bytes. */
			//short int sample5 = DataTools::getBytesValue <short int> (&data[i], 2);
			//std::cout<<"Sample5 value: "<<sample5<<".\n";
				
				//std::cout<<"Sample2 value: "<<sample2<<".\n";
				//std::string binary4 = DataTools::getBinaryString <short int> (sample5);
				//std::cout<<"Sample5 binary: "<<binary4<<".\n";
				
				
				//std::string binary2 = DataTools::getBinaryString <char> (data[i]);
				//std::string binary3 = DataTools::getBinaryString <char> (data[i+1]);
				//std::cout<<"Final binary values: "<<binary2<<" and "<<binary3<<".\n";
				//std::cout<<"***\n";
				//int blah = data[i];
				// data[i]=0;
				// data[i+1]=0;
				
				// std::cout<<"Sizeof int: "<<sizeof(int)<<".\n";
				// int sample=0;
				// sample =sample|data[i];
				// sample=sample<<8;
				// sample=sample|data[i+1];
				// std::cout<<"Bitshifted sample int: "<<sample<<".\n";
				//sample[i]=<<sample[i];
				//sample=sample|data[i+1];
				//int sample = data[i];
				//int endSample = smoother.Process(sample);
				//data[i] = endSample;
				//if(i>100) { break; }
			}
			// for (int i=2;i<nData;i+=bytesPerSample*nChannels)
			// {
			
				// data[i]=0;
				// data[i+1]=0;
				// //int sample = data[i];
				// //int endSample = smoother.Process(sample);
				// //data[i] = endSample;
			// }
			
			//int sample = 
			
		}
		
		// for(int i=0;i<nData;i+=byteSize)
		// {
			// //data2
		// }
		
		
		// for (int i=0;i<nData-1;++i)
		// {
			// data2[i]=(data[i]+data[i+1])/2;
			// std::cout<<"Filtered: "<<(int)data2[i]<<" from "<<(int)data[i]<<","<<(int)data[i+1]<<"\n";
		// }
		// data2[nData-1]=data[nData-1];
		
		// for (int i=0;i<nData;++i)
		// {
			// data[i]=data2[i];
		// }
	
	
		// CParamSmooth smoothie;

		// for (int i=0;i<nData;++i)
		// {
		
			// // float iData = data[i];
			// // iData = smoothie.Process(data[i]);
			// // if(iData<0) { iData=0; }
			// // if(iData>255) { iData=255; }
		// data[i] = smoothie.Process(data[i]);
		// //	data[i]=(char)iData;
		// }
	}
	
	
	/* 16 bit values have been passed. */
	void loadRawData (char* _data, int _nData)
	{
		// std::cout<<"nData: "<<_nData<<"\n";
		
		// std::cout<<"Preview of initial data:\n";
		// for(int i=0;i<20;++i)
		// {
			// std::cout<<_data[i]<<",";
		// } std::cout<<"\n";
		// std::cout<<"Preview of converted data:\n";
		
		nData=_nData;
		data = new char [nData];
		
		// char* convData = (char*)_data;
		// for(int i=0;i<20;++i)
		// {
			// std::cout<<(int)convData[i]<<",";
		// } std::cout<<"\n";
		
		
		for(int i=0;i<nData;++i)
		{
			data[i]=_data[i];
		}
		
	}
	/* 8 bit values have been passed. */
	// void loadRawData8 (char* _data, int _nData)
	// {
		// nData=_nData;
		// data = new char [_nData];
		
		// for(int i=0;i<nData;++i)
		// {
			// data[i]=_data[i];
		// }
		
	// }
	// void loadRawData16 (short int* _data, int _nData)
	// {
		// nData=_nData;
		// data16 = new char [_nData];
		
		// for(int i=0;i<nData;++i)
		// {
			// data16[i]=_data[i];
		// }
		
	// }
	
	void compress(const double multiplier)
	{
		for(int i=0;i<nData;++i)
		{
			data[i]/=multiplier;
		}
	}
	
	void compress2(const double multiplier)
	{
		for(int i=0;i<nData;++i)
		{
			unsigned char val = (unsigned char) data[i];
			val/=multiplier;
			data[i]=(char)val;
		}
	}
	
	
	double getAmplitudeFast()
	{
		/* Get 100 amplitude samples. */
		double amplitude[100];
		
		if(nData<100)
		{
			return 0;
		}
	
		for(int i=0;i<nData-1;++i)
		{
			if(i==100)
			{
				break;
			}
			else
			{
				amplitude[i]=data[i]-data[i+1];
				if(amplitude[i]<0) { amplitude[i]*=-1; }
			}
		}
		double total=0;
		for(int i=0;i<100;++i)
		{
			total+=amplitude[i];
		}
		total/=100;
		return total;
		
		
	
	}
	
	void fillAll(int value)
	{
		for(int i=0;i<=nData;++i)
		{
			data[i]=value;
		}
	}
	void whiteNoise(int _samples=100)
	{
	
	// int nChannels; /* How many audio tracks there are... Typically 1 for mono and 2 for stereo, though it can be more complicated than that. */
	// int bitsPerSample; /* How many bits there are to store one piece of audio data. For this class only 8 and 16 are valid. */
	// double durationSeconds; /* How long the audio clip lasts for. */
	// //short int * data16; /* The raw audio data for 16 bit audio */
	// char * data; /* The raw audio data. */
	// int nData; /* The size of the data array. Not to be confused with the number of bytes of data. */
	// int byteRate; /* How many bytes of data are contained for a second of audio. */
	// int samplesPerSecond; /* Samples per second for each channel. Also known as hertz. */
	
	
		// nChannels=2;
		// bitsPerSample=8;
		
		// data[i]
	
		for(int i=0;i<nData;++i)
		{
			data[i]=random.randomInt(_samples);
		}

	}

	void printData(int n=-1)
	{
	std::cout<<"NDATA: "<<nData<<".\n";
		std::cout<<"Printing sound data:\n";
		for(int i=0;i<nData;++i)
		{
			std::cout<<(int)data[i]<<",";
			if(i==n)
			{
				break;
			}
		}
		std::cout<<"\nEnd printing sound data.\n";
	}
	
	void printDebugInfo()
	{
		std::cout<<"***Sound::printDebugInfo()\n";
		
		std::cout<<"nChannels: "<<nChannels<<".\n";
		std::cout<<"bitsPerSample: "<<bitsPerSample<<".\n";
		std::cout<<"durationSeconds: "<<durationSeconds<<".\n";
		std::cout<<"nData: "<<nData<<".\n";
		std::cout<<"byteRate: "<<byteRate<<".\n";
		std::cout<<"samplesPerSecond: "<<samplesPerSecond<<".\n";
		
		std::cout<<"***END Sound::printDebugInfo()\n";
	}
	
};




#endif /* #ifndef AUDIO_SOUND_HPP */
