#pragma once
#ifndef WILDCAT_AUDIO_AUDIO_PLAYER_HPP
#define WILDCAT_AUDIO_AUDIO_PLAYER_HPP

/* Wildcat: AudioPlayer
   #include <Audio/AudioPlayer.hpp>
   
   Provides generic functions to play sounds independently of audio library.
   
   Currently only OpenAL is supported.
   
   Note that WILDCAT_AUDIO must be defined in order for any audio to play.
*/

class AudioPlayer
{
	public:
	
	/* 0: mute, 100: max. */
	int globalVolume;
	int maxSounds;
	
	//Vector <Audio*> vAudio;
	
	
	virtual void init()=0;
	/* Play the sound once and then stop. */
	virtual void playSoundOnce(Sound*) {};
	/* Same as above, but ensures that the audio player or sound card isn't overwhelmed with too many of the same sound being played at once. */
	virtual void playSoundOnce(Sound*,int /* Maximum instances. */) {};
	
	/* Playing audio can lag if the sound isn't prepared earlier. This function is used to prepare a sound to be played later. */
	virtual void preloadSound(Sound*){};
	
	/* Free up memory and close audio devices, etc. */
	virtual void close() {};
};

#endif /* #ifndef WILDCAT_AUDIO_AUDIO_PLAYER_HPP */
