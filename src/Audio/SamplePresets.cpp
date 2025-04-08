#include <GameEngineFramework/Audio/SamplePresets.h>

/// Generate a square wave
void AudioPreset::RenderBlankSpace(AudioSample* sample, float duration) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * duration);
    
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back(0);
    
    return;
}

/// Generate white noise
void AudioPreset::RenderWhiteNoise(AudioSample* sample, float duration) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * duration);
    
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back( (int32_t)Random.Range(-32767, 32767) );
    
    return;
}

/// Generate square waves
void AudioPreset::RenderSquareWave(AudioSample* sample, float frequency, float duration) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * duration);
    int period = (int)(sample->sample_rate / frequency) + 10;
    
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back( (i % period < (period / 2)) ? 32767 : -32768 );
    
    return;
}


void AudioPreset::RenderSweepingSineWave(AudioSample* sample, double startFreq, double endFreq, double duration) {
    int totalSamples = (int)(duration * sample->sample_rate);
    double freqDelta = (endFreq - startFreq) / totalSamples;
    
    for (int i = 0; i < totalSamples; i++) {
        double t = (double)(i) / sample->sample_rate;
        double frequency = startFreq + freqDelta * i;
        
        sample->sampleBuffer.push_back( (int32_t)(32767 * std::sin(2 * 3.14159f * frequency * t)) );
    }
    return;
}

