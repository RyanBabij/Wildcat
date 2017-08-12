#pragma once
#ifndef AUDIO_PLAYER_OPENAL_HPP
#define AUDIO_PLAYER_OPENAL_HPP

#include <Audio/OpenAL/al.h>
#include <Audio/OpenAL/alc.h>
#include <Audio/OpenAL/alext.h>

#include <Audio/AudioPlayer.hpp>
#include <Container/Vector/Vector.hpp>



/*
#include <Audio/AudioPlayer_OpenAL.hpp>
OpenAL implementation of AudioPlayer.
*/

#define WILCAT_AUDIO

class AudioPlayer_OpenAL: public AudioPlayer
{
	public:
#ifdef WILCAT_AUDIO
	ALCdevice* alcDevice;
	ALCcontext* alcContext;
	
	//bool audioInitialised;
	
	Vector <Sound*> vSound;
	Vector <ALuint> vSource;
#endif
	
	AudioPlayer_OpenAL()
	{
#ifdef WILCAT_AUDIO
		alcDevice=0;
		alcContext=0;
		globalVolume=50;
#endif
	}
	
	/* ALC needs to be shut down properly or it seems to hang the system if audio is playing. */
	~AudioPlayer_OpenAL()
	{
#ifdef WILCAT_AUDIO
		close();
#endif
	}
	
	/* Sets up OpenAL so it can play sounds. */
	/* Todo: Implement error handling. */
	void init()
	{
#ifdef WILCAT_AUDIO
		// if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
		// {
			// std::cout<<"ALC devie enumeration extension found.\n";
			// std::string strDevices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
			// for(int i=0;;++i)
			// {
				// if(strDevices[i]=='\0')
				// {
					// std::cout<<"nl";
					// std::cout<<"\n";
					// ++i;
					// if(strDevices[i]=='\0')
					// {
						// std::cout<<"dn";
						// break;
					// }
				// }
				// std::cout<<strDevices[i];
			// }
			// std::cout<<strDevices<<".\n";
		// }
	
	
		alcDevice=alcOpenDevice(0);
		if(alcDevice!=0)
		{
			alcContext=alcCreateContext(alcDevice,NULL); 
			alcMakeContextCurrent(alcContext);
		}
		//audioInitialised=true;
#endif
	}
	
	void close()
	{
#ifdef WILCAT_AUDIO
		alcMakeContextCurrent(NULL); 
		alcDestroyContext(alcContext); 
		alcCloseDevice(alcDevice);
#endif
	}

	/* Buffer the sound to reduce time taken to play it. All sound playing functions automatically check to see if the sound is preloaded. */
	void preloadSound(Sound* sound)
	{
#ifdef WILCAT_AUDIO

#endif
	}
	
	/* Plays sound in a thread, and doesn't loop. */
	void playSoundOnce(Sound* sound)
	{
#ifdef WILCAT_AUDIO
		ALuint soundBuffer;
		alGenBuffers(1, &soundBuffer);
		
		//std::cout<<"sound->bitsPerSample: "<<sound->bitsPerSample<<".\n";
		
		if(sound->bitsPerSample==8)
		{
			//std::cout<<"Nchannels: "<<sound->nChannels<<".\n";
			//std::cout<<"8 bit.\n";
			if(sound->nChannels==1)
			{ alBufferData(soundBuffer,AL_FORMAT_MONO8,sound->data,sound->nData,sound->samplesPerSecond); }
			else if (sound->nChannels==2)
			{ alBufferData(soundBuffer,AL_FORMAT_STEREO8,sound->data,sound->nData,sound->samplesPerSecond); }
			else
			{ std::cout<<"Error: WAV needs to have 1 or 2 channels.\n"; }
		}
		else if(sound->bitsPerSample==16)
		{
			//std::cout<<"Nchannels: "<<sound->nChannels<<".\n";
			//std::cout<<"16 bit.\n";
			if(sound->nChannels==1)
			{ alBufferData(soundBuffer,AL_FORMAT_MONO16,sound->data,sound->nData,sound->samplesPerSecond); }
			else if (sound->nChannels==2)
			{ alBufferData(soundBuffer,AL_FORMAT_STEREO16,sound->data,sound->nData,sound->samplesPerSecond); }
			else
			{ std::cout<<"Error: WAV needs to have 1 or 2 channels.\n"; }
		}
		else
		{ std::cout<<"Error: Sound needs to be 8 or 16 bit.\n"; }

		
		ALuint source;
		alGenSources(1,&source);
		/* Load buffer into source. */
		alSourcei(source, AL_BUFFER, soundBuffer);
		/* Set volume. */
		const float volume = (float)globalVolume/100;
		alSourcef(source, AL_GAIN, volume);
		/* Play source in new thread. */
		alSourcePlay(source);
		
		vSound.push(sound);
		vSource.push(source);
		
#endif
	}
	
	void playSoundOnce (Sound* sound, const int maxInstances)
	{
#ifdef WILCAT_AUDIO
		//std::cout<<"Playing sound with max "<<maxInstances<<" instances.\n";
		
#endif
	}
	
	void playSoundLoop(Sound* sound)
	{
#ifdef WILCAT_AUDIO
		ALuint soundBuffer;
		alGenBuffers(1, &soundBuffer);
		
		if(sound->bitsPerSample==8)
		{
			if(sound->nChannels==1)
			{ alBufferData(soundBuffer,AL_FORMAT_MONO8,sound->data,sound->nData,sound->samplesPerSecond); }
			else if (sound->nChannels==2)
			{ alBufferData(soundBuffer,AL_FORMAT_STEREO8,sound->data,sound->nData,sound->samplesPerSecond); }
			else
			{ std::cout<<"Error: WAV needs to have 1 or 2 channels.\n"; }
		}
		else if(sound->bitsPerSample==16)
		{
			//std::cout<<"Playing 16 bit sound.\n";
			if(sound->nChannels==1)
			{ alBufferData(soundBuffer,AL_FORMAT_MONO16,sound->data,sound->nData,sound->samplesPerSecond); }
			else if (sound->nChannels==2)
			{ alBufferData(soundBuffer,AL_FORMAT_STEREO16,sound->data,sound->nData,sound->samplesPerSecond); }
			else
			{ std::cout<<"Error: WAV needs to have 1 or 2 channels.\n"; }
		}
		else
		{ std::cout<<"Error: Sound needs to be 8 or 16 bit.\n"; }
		
		ALuint source;
		alGenSources(1,&source);
		/* Load buffer into source. */
		alSourcei(source, AL_BUFFER, soundBuffer);
		/* Set to loop. */
		alSourcei(source, AL_LOOPING, AL_TRUE);
		/* Set volume. */
		const float volume = (float)globalVolume/100;
		alSourcef(source, AL_GAIN, volume);
		/* Play source in new thread. */
		alSourcePlay(source);
		
		
		vSound.push(sound);
		vSource.push(source);
#endif
	}
	
	void stopSound(Sound* sound)
	{
#ifdef WILCAT_AUDIO
		int nStops=0;
		for(int i=0;i<vSound.size();++i)
		{
			if(vSound(i)==sound)
			{
				alSourceStop(vSource(i));
				++nStops;
			}
		}
	//	std::cout<<"Num stops: "<<nStops<<".\n";
#endif
	}
	void stopAllSounds()
	{
#ifdef WILCAT_AUDIO
		for(int i=0;i<vSound.size();++i)
		{
			//alSourcei(vSource(i), AL_LOOPING, AL_FALSE);
			//stopSound(vSound(i));
			//std::cout<<"Stopping.\n";
			alSourceStop(vSource(i));
		}
#endif
	}
	
	void garbageCollect()
	{
		
	}
	
};


#endif /* #ifndef AUDIO_PLAYER_OPENAL_HPP */
