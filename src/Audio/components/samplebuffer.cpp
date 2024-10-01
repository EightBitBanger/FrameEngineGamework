#include <GameEngineFramework/Audio/components/samplebuffer.h>


/// Generate a square wave
void AudioPreset::renderBlankSpace(AudioSample* sample, float duration) {
    int samples = (int)(sample->sample_rate * duration);
    
    for (int i = 0; i < samples; i++) 
        sample->sampleBuffer.push_back( Random.Range(-1, 1) );
    
    return;
}

/// Generate white noise
void AudioPreset::renderWhiteNoise(AudioSample* sample, float duration) {
    int samples = (int)(sample->sample_rate * duration);
    
    for (int i = 0; i < samples; i++) 
        sample->sampleBuffer.push_back( (ALshort)Random.Range(32767, -32767) );
    
    return;
}

/// Generate square waves
void AudioPreset::renderSquareWave(AudioSample* sample, float frequency, float duration) {
    int samples = (int)(sample->sample_rate * duration);
    int period = (int)(sample->sample_rate / frequency);
    
    for (int i = 0; i < samples; i++) 
        sample->sampleBuffer.push_back((i % period < (period / 2)) ? 32767 : -32768);
    
    return;
}

void AudioPreset::renderSweepingSineWave(AudioSample* sample, double startFreq, double endFreq, double duration) {
    
    int totalSamples = (int)(duration * sample->sample_rate);
    
    double freqDelta = (endFreq - startFreq) / totalSamples;
    
    for (int i = 0; i < totalSamples; i++) {
        double t = (double)(i) / sample->sample_rate;
        
        double frequency = startFreq + freqDelta * i;
        
        sample->sampleBuffer.push_back( (ALshort)(32767 * std::sin(2 * 3.14159f * frequency * t)) );
    }
    
    return;
}

