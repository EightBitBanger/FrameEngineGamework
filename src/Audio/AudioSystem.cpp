#include <GameEngineFramework/Audio/AudioSystem.h>

extern Logger Log;
extern NumberGeneration Random;

// Audio thread
bool isAudioThreadActive = true;
void AudioThreadMain(void);

// Master final mix buffer
std::vector<int16_t> mixBuffer(512);

void AudioSystem::Initiate(void) {
    
    isAudioThreadActive = true;
    
    // Launch the audio thread
    audioThread = new std::thread(AudioThreadMain);
    Log.Write(" >> Starting thread audio");
    
    SDL_Init(SDL_INIT_AUDIO);
    
    // Initiate the audio stream
    SDL_AudioSpec spec = {};
    spec.freq = 44100;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    
    mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_ResumeAudioStreamDevice(mStream);
    
    return;
}


void AudioSystem::Shutdown(void) {
    
    isAudioThreadActive = false;
    
    audioThread->join();
    
    return;
}

Sound* AudioSystem::CreateSound(void) {
    Sound* newSoundPtr = mSounds.Create();
    return newSoundPtr;
}

bool AudioSystem::DestroySound(Sound* soundPtr) {
    return mSounds.Destroy(soundPtr);
}

AudioSample* AudioSystem::CreateAudioSample(void) {
    AudioSample* newSample = mSamples.Create();
    return newSample;
}

bool AudioSystem::DestroyAudioSample(AudioSample* samplePtr) {
    return mSamples.Destroy(samplePtr);
}

bool AudioSystem::Play(Sound* soundPtr) {
    if (soundPtr->isPlaying) 
        return false;
    mActiveSounds.push_back(soundPtr);
    soundPtr->isPlaying = true;
    return true;
}

//
// Audio thread entry point
//

extern AudioSystem Audio;

void AudioThreadMain(void) {
    
    while (isAudioThreadActive) {
        
        //std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        std::lock_guard<std::mutex> lock(Audio.mux);
        
        // Check if the buffer is low
        int available = SDL_GetAudioStreamAvailable(Audio.mStream);
        if (available > 1024) 
            continue;
        
        // Temporary mix buffer
        std::vector<int32_t> buffer;
        buffer.resize(512); // 512 samples
        
        // Mix the currently playing sounds
        Audio.MixActiveSounds(buffer);
        
        // Send in the next section of mixed audio
        SDL_PutAudioStreamData(Audio.mStream, buffer.data(), buffer.size() * sizeof(int16_t));
        //SDL_FlushAudioStream(Audio.mStream);
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(2) );
    Log.Write(" >> Shutting down on thread audio");
    
    return;
}



void AudioSystem::MixActiveSounds(std::vector<int32_t>& buffer) {
    std::fill(buffer.begin(), buffer.end(), 0);
    
    for (unsigned int i=0; i < mActiveSounds.size(); i++) {
        Sound* soundPtr = mActiveSounds[i];
        std::lock_guard<std::mutex> lock(soundPtr->mux);
        
        // Check sound has stopped playing
        if (!soundPtr->isPlaying) {
            mActiveSounds.erase(mActiveSounds.begin() + i);
            continue;
        }
        
        // Get the audio sample
        std::vector<int16_t>& sample = soundPtr->sample->sampleBuffer;
        
        // Mix the next section of samples
        for (unsigned int s = 0; s < buffer.size(); s++) {
            
            // Check sound has ended
            if (soundPtr->playbackCursor >= sample.size()) {
                soundPtr->Stop();
                mActiveSounds.erase(mActiveSounds.begin() + i);
                break;
            }
            
            // Mix the audio
            int32_t mixed = buffer[s] + static_cast<int32_t>(sample[soundPtr->playbackCursor]);
            soundPtr->playbackCursor++;
            
            // Cap the audio
            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;
            
            buffer[s] = mixed;
        }
        
        continue;
    }
    return;
}
