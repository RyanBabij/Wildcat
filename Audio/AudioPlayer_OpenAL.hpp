#pragma once
#ifndef WILDCAT_AUDIO_AUDIO_PLAYER_OPENAL_HPP
#define WILDCAT_AUDIO_AUDIO_PLAYER_OPENAL_HPP

/* Wildcat: AudioPlayer_OpenAL
   #include <Audio/AudioPlayer_OpenAL.hpp>
   
   OpenAL implementation of AudioPlayer.
   
   Note that WILDCAT_AUDIO must be defined in order for any audio to play.
*/

#include <Audio/OpenAL/al.h>
#include <Audio/OpenAL/alc.h>
#include <Audio/OpenAL/alext.h>

#include <Audio/AudioPlayer.hpp>
#include <Container/Vector/Vector.hpp>


// load a bunch of sources to allow overlapping sounds
class ALRevolver
{
   public:
#ifdef WILDCAT_AUDIO
   Vector <ALuint> vLoadedSource;
   
   int currentIndex;
#endif
   
   ALRevolver()
   {
#ifdef WILDCAT_AUDIO
      currentIndex=0;
#endif
   }
   
   void load(Sound* sound, int nDupes)
   {
#ifdef WILDCAT_AUDIO
      
      for (int i=0;i<nDupes;++i)
      {

         ALuint soundBuffer;
         alGenBuffers(1, &soundBuffer);
         
         ALuint source;
         alGenSources(1,&source);
		
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
            if(sound->nChannels==1)
            { alBufferData(soundBuffer,AL_FORMAT_MONO16,sound->data,sound->nData,sound->samplesPerSecond); }
            else if (sound->nChannels==2)
            { alBufferData(soundBuffer,AL_FORMAT_STEREO16,sound->data,sound->nData,sound->samplesPerSecond); }
            else
            { std::cout<<"Error: WAV needs to have 1 or 2 channels.\n"; }
         }
         else
         { std::cout<<"Error: Sound needs to be 8 or 16 bit.\n"; }

         

         /* Load buffer into source. */
         alSourcei(source, AL_BUFFER, soundBuffer);
         /* Set volume. */
         //const float volume = (float)globalVolume/100;
         //alSourcef(source, AL_GAIN, volume);
         alSourcef(source, AL_GAIN, 10);

         vLoadedSource.push(source);
         //return vLoadedSource.size()-1;
         // vSound.push(sound);
         // vSource.push(source);
         
         
         
      }
		
#endif
   }
   
   void playRotate()
   {
#ifdef WILDCAT_AUDIO
 //alSourceStop(vLoadedSource(currentIndex));
      ++currentIndex;
      if (currentIndex >= vLoadedSource.size() )
      {
         currentIndex=0;
      }
		/* Play source in new thread. */
       std::cout<<"Playing: "<<currentIndex<<" with id "<<vLoadedSource(currentIndex)<<"\n";
		alSourcePlay(vLoadedSource(currentIndex));
      
     
#endif
   }
   
};

class AudioPlayer_OpenAL: public AudioPlayer
{
	public:
#ifdef WILDCAT_AUDIO
	ALCdevice* alcDevice;
	ALCcontext* alcContext;
	
	//bool audioInitialised;
	
	Vector <Sound*> vSound;
	Vector <ALuint> vSource;
	Vector <ALuint> vBuffer;
	
	
	bool closed; /* Prevent closing multiple times which can cause crash */
   
   Vector <ALuint> vLoadedSource;
   Vector <ALuint> vLoadedBuffer;
   
   Vector <ALRevolver*> vRevolver;
   
#endif
	
	AudioPlayer_OpenAL()
	{
#ifdef WILDCAT_AUDIO
		alcDevice=0;
		alcContext=0;
		globalVolume=50;
		closed = false;
#endif
	}
   
   int loadRevolver(Sound * sound, int nSounds)
   {
#ifdef WILDCAT_AUDIO
      ALRevolver * alv = new ALRevolver();
      alv->load(sound,nSounds);
      vRevolver.push(alv);
      return vRevolver.size()-1;
#else
   return 0;
#endif
   }
   
   void playRevolver(int id)
   {
#ifdef WILDCAT_AUDIO
      vRevolver(id)->playRotate();
#endif
   }
   
   int load(Sound* sound)
   {
#ifdef WILDCAT_AUDIO
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

      vLoadedSource.push(source);
      return vLoadedSource.size()-1;
		// vSound.push(sound);
		// vSource.push(source);
#else
   return 0;
#endif
   }
	
   void playLoaded(int id)
   {
#ifdef WILDCAT_AUDIO
      alSourceStop(vLoadedSource(id));
		/* Play source in new thread. */
		alSourcePlay(vLoadedSource(id));

#endif
   }
   
	/* ALC needs to be shut down properly or it seems to hang the system if audio is playing.
		In some cases the audio player needs to be closed manually */
	~AudioPlayer_OpenAL()
	{
#ifdef WILDCAT_AUDIO
		close();
#endif
	}
	
	/* Sets up OpenAL so it can play sounds. */
	/* Todo: Implement error handling. */
	void init()
	{
#ifdef WILDCAT_AUDIO
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
#ifdef WILDCAT_AUDIO
		if (closed == false)
		{
			alcMakeContextCurrent(NULL); 
			alcDestroyContext(alcContext); 
			alcCloseDevice(alcDevice);
			closed = true;
		}
#endif
	}

	/* Buffer the sound to reduce time taken to play it. All sound playing functions automatically check to see if the sound is preloaded. */
	void preloadSound(Sound* sound)
	{
#ifdef WILDCAT_AUDIO

#endif
	}
	
	/* Plays sound in a thread, and doesn't loop. */
	void playSoundOnce(Sound* sound)
	{
#ifdef WILDCAT_AUDIO
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
		vBuffer.push(soundBuffer);
		
#endif
	}
	
	void playSoundOnce (Sound* sound, const int maxInstances)
	{
#ifdef WILDCAT_AUDIO
		//std::cout<<"Playing sound with max "<<maxInstances<<" instances.\n";
		
#endif
	}
	
	void playSoundLoop(Sound* sound)
	{
#ifdef WILDCAT_AUDIO
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
#ifdef WILDCAT_AUDIO
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
#ifdef WILDCAT_AUDIO
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
#ifdef WILDCAT_AUDIO
// This should be called periodically to free up memory from sounds that have finished playing
         for (int i=0;i<vSource.size();++i)
         {
            ALenum state;
            alGetSourcei(vSource(i), AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
            {
               alDeleteSources(1,&vSource(i));
               alDeleteBuffers(1,&vBuffer(i));
               vSource(i)=0;
               vBuffer(i)=0;
            }
         }
         vSource.removeNulls();
         vBuffer.removeNulls();
#endif
	}
	
};


#endif /* #ifndef AUDIO_PLAYER_OPENAL_HPP */
