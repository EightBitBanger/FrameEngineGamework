#include <GameEngineFramework/Audio/AudioSystem.h>

//#include <cstring>
//#include <iostream>
//#include <audioclient.h>
//#include <mmdeviceapi.h>
//#include <wrl.h>
//using namespace Microsoft::WRL;


extern Logger Log;
extern NumberGeneration Random;


// Audio thread
bool isAudioDeviceActive = false;
bool isAudioThreadActive = true;
void AudioThreadMain(void);





void AudioSystem::Initiate(void) {
    
    // Launch the audio thread
    audioThread = new std::thread( AudioThreadMain );
    
    Log.Write( " >> Starting thread audio" );
    
    mDevice = alcGetContextsDevice(mContext);
    
    if (!mDevice) 
        mDevice = alcOpenDevice(nullptr);
    
    if (mDevice) {
        
        mContext = alcGetCurrentContext();
        
        if (!mContext) {
            
            mContext = alcCreateContext(mDevice, nullptr);
            
            alcMakeContextCurrent(mContext);
            
            isAudioDeviceActive = true;
        }
        
    } else {
        
        Log.Write("!! Unable to open audio device.");
        Log.WriteLn();
    }
    
    /*
    
    //
    // WASAPI testing
    //
    
    const int SAMPLE_RATE = 44100;  // Sample rate in Hz
    const int DURATION = 5;          // Duration in seconds
    const float FREQUENCY = 440.0f;  // Frequency of the sine wave in Hz
    const int NUM_SAMPLES = SAMPLE_RATE * DURATION;
    
    void GenerateSineWave(float* buffer, int numSamples, float frequency) {
        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = 0.5f * sinf(2.0f * 3.14159f * frequency * i / SAMPLE_RATE);
        }
    }
    
    CoInitialize(nullptr);

    // Create the device enumerator
    ComPtr<IMMDeviceEnumerator> deviceEnumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create device enumerator." << std::endl;
        return;
    }

    // Get the default audio endpoint
    ComPtr<IMMDevice> device;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get default audio endpoint." << std::endl;
        return;
    }

    // Activate the audio client
    ComPtr<IAudioClient> audioClient;
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)(&audioClient));
    if (FAILED(hr))
    {
        std::cerr << "Failed to activate audio client." << std::endl;
        return;
    }

    // Get the mix format
    WAVEFORMATEX* waveFormat;
    hr = audioClient->GetMixFormat(&waveFormat);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get mix format." << std::endl;
        return;
    }

    // Initialize the audio client
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, waveFormat, nullptr);
    if (FAILED(hr))
    {
        CoTaskMemFree(waveFormat);
        std::cerr << "Failed to initialize audio client." << std::endl;
        return;
    }

    // Get the render client
    ComPtr<IAudioRenderClient> renderClient;
    hr = audioClient->GetService(IID_PPV_ARGS(&renderClient));
    if (FAILED(hr))
    {
        std::cerr << "Failed to get render client." << std::endl;
        CoTaskMemFree(waveFormat);
        return;
    }

    // Allocate a buffer for the audio data
    UINT32 bufferFrameCount;
    hr = audioClient->GetBufferSize(&bufferFrameCount);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get buffer size." << std::endl;
        CoTaskMemFree(waveFormat);
        return;
    }

    // Create the audio buffer
    float* audioBuffer = new float[NUM_SAMPLES];
    GenerateSineWave(audioBuffer, NUM_SAMPLES, FREQUENCY);

    // Prepare for playback
    for (int i = 0; i < DURATION; ++i)
    {
        UINT32 numFramesPadding;
        hr = audioClient->GetCurrentPadding(&numFramesPadding);
        UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Determine how much audio we can write
        UINT32 numFramesToWrite = (NUM_SAMPLES < numFramesAvailable) ? NUM_SAMPLES : numFramesAvailable;

        // Get a pointer to the audio buffer
        BYTE* pData;
        hr = renderClient->GetBuffer(numFramesToWrite, &pData);
        if (FAILED(hr))
        {
            std::cerr << "Failed to get audio buffer." << std::endl;
            break;
        }

        // Copy the sine wave data to the buffer
        memcpy(pData, audioBuffer + (i * bufferFrameCount), numFramesToWrite * sizeof(float));

        // Release the buffer
        hr = renderClient->ReleaseBuffer(numFramesToWrite, 0);
        if (FAILED(hr))
        {
            std::cerr << "Failed to release audio buffer." << std::endl;
            break;
        }

        // Sleep for the duration of one buffer
        Sleep(1000 / (SAMPLE_RATE / bufferFrameCount));
    }

    // Stop the audio client
    audioClient->Stop();
    std::cout << "Playback stopped." << std::endl;

    // Cleanup
    delete[] audioBuffer;
    CoTaskMemFree(waveFormat);
    CoUninitialize();
    
    */
    
    
    
    
    return;
}

void AudioSystem::Shutdown(void) {
    
    alcMakeContextCurrent(nullptr);
    
    if (mContext != 0) 
        alcDestroyContext(mContext);
    
    if (mDevice != 0) 
        alcCloseDevice(mDevice);
    
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

bool AudioSystem::CheckIsAudioEndpointActive(void) {
    
    return mIsDeviceActive;
}

//
// Audio thread entry point
//

void AudioThreadMain(void) {
    
    while (isAudioThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(2) );
    Log.Write(" >> Shutting down on thread audio");
    
    return;
}


