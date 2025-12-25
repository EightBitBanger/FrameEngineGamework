#include <GameEngineFramework/Audio/SamplePresets.h>
#include <GameEngineFramework/Math/Random.h>
#include <cmath>
#include <cstdint>
#include <algorithm>


extern NumberGeneration Random;

void AudioPreset::RenderBlankSpace(AudioSample* sample, float seconds) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * seconds);
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back(0);
}

void AudioPreset::RenderWhiteNoise(AudioSample* sample, float seconds) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * seconds);
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back( (int32_t)Random.Range(-32767, 32767) );
}

void AudioPreset::RenderSquareWave(AudioSample* sample, float frequency, float seconds) {
    unsigned int totalSamples = (unsigned int)(sample->sample_rate * seconds);
    int period = (int)(sample->sample_rate / frequency) + 10;
    for (unsigned int i = 0; i < totalSamples; i++) 
        sample->sampleBuffer.push_back( (i % period < (period / 2)) ? 32767 : -32768 );
}

void AudioPreset::RenderSweepingSineWave(AudioSample* sample, double startFreq, double endFreq, double seconds) {
    int totalSamples = (int)(seconds * sample->sample_rate);
    double freqDelta = (endFreq - startFreq) / totalSamples;
    
    for (int i = 0; i < totalSamples; i++) {
        double t = (double)(i) / sample->sample_rate;
        double frequency = startFreq + freqDelta * i;
        
        sample->sampleBuffer.push_back( (int32_t)(32767 * std::sin(2 * 3.14159f * frequency * t)) );
    }
}


static inline uint32_t XorShift32(uint32_t& state) {
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

static inline float RandFloatSigned(uint32_t& state) {
    // [-1, 1]
    uint32_t v = XorShift32(state);
    // 24-bit mantissa-ish
    float f = (float)(v & 0x00FFFFFFu) / (float)0x00FFFFFFu;
    return (f * 2.0f) - 1.0f;
}

static inline float Clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float SoftClip(float x) {
    return std::tanh(x);
}

// Simple resonator (2-pole) that approximates a bandpass / formant peak.
struct Resonator {
    float a1, a2, b0;
    float y1, y2;
    
    Resonator() : a1(0), a2(0), b0(1), y1(0), y2(0) {}
    
    void Set(float sampleRate, float freqHz, float bandwidthHz) {
        // Clamp to safe ranges
        if (freqHz < 10.0f) freqHz = 10.0f;
        if (freqHz > sampleRate * 0.45f) freqHz = sampleRate * 0.45f;
        
        if (bandwidthHz < 5.0f) bandwidthHz = 5.0f;
        
        // Pole radius from bandwidth (rough but effective)
        // r = exp(-pi * BW / Fs)
        float r = std::exp(-3.14159265f * bandwidthHz / sampleRate);
        float w = 2.0f * 3.14159265f * (freqHz / sampleRate);
        
        a1 = 2.0f * r * std::cos(w);
        a2 = -(r * r);
        
        // Feed gain: scale so peak isn’t tiny. This is heuristic.
        b0 = (1.0f - r);
    }
    
    inline float Process(float x) {
        float y = b0 * x + a1 * y1 + a2 * y2;
        y2 = y1;
        y1 = y;
        return y;
    }
};

static inline float EnvelopeAHRS(float t, float attack, float hold, float release, float total) {
    // A -> H -> R (no sustain level separate; animal vocal “barks” like this)
    if (total <= 0.0f) return 0.0f;
    
    if (attack < 0.0001f) attack = 0.0001f;
    if (release < 0.0001f) release = 0.0001f;
    if (hold < 0.0f) hold = 0.0f;
    
    float endAttack = attack;
    float endHold   = attack + hold;
    float startRel  = std::max(endHold, total - release);
    
    if (t < endAttack) {
        float a = t / endAttack;
        return Clamp01(a);
    }
    if (t < startRel) {
        return 1.0f;
    }
    if (t < total) {
        float r = (t - startRel) / (total - startRel);
        return Clamp01(1.0f - r);
    }
    return 0.0f;
}

void AudioPreset::RenderAnimalVocal(AudioSample* sample, const VocalParameters& params) {
    if (sample == nullptr) return;
    
    const float sr = (float)sample->sample_rate;
    if (sr <= 0.0f) return;
    
    const float seconds = (params.seconds > 0.0f) ? params.seconds : 0.25f;
    const int totalSamples = (int)(seconds * sr);
    if (totalSamples <= 0) return;
    
    // Seed
    uint32_t rng = params.seed;
    if (rng == 0u) {
        // Cheap variability if user doesn’t care
        rng = 0xA53C9E1Du ^ (uint32_t)(totalSamples * 2654435761u);
    }
    
    // Prepare sample buffer
    sample->sampleBuffer.clear();
    sample->sampleBuffer.reserve((size_t)totalSamples);
    
    // Formant resonators
    Resonator r1, r2, r3;
    r1.Set(sr, params.formant1Hz, params.bandwidth1Hz);
    r2.Set(sr, params.formant2Hz, params.bandwidth2Hz);
    r3.Set(sr, params.formant3Hz, params.bandwidth3Hz);
    
    // A tiny one-pole lowpass for noise “breath” smoothing
    float noiseLP = 0.0f;
    const float noiseCut = 0.18f; // fixed “air” tone; tweak if you want
    const float noiseA = Clamp01(noiseCut);
    
    // Phase accumulator
    float phase = 0.0f;
    
    // Harmonic stack control
    float brightness = Clamp01(params.brightness);
    int harmonics = 6 + (int)(brightness * 18.0f); // 6..24
    if (harmonics < 1) harmonics = 1;
    
    for (int i = 0; i < totalSamples; ++i) {
        float t = (float)i / sr;
        float u = (totalSamples > 1) ? ((float)i / (float)(totalSamples - 1)) : 0.0f;
        
        // Pitch: glide + vibrato + jitter
        float baseHz = Lerp(params.pitchStartHz, params.pitchEndHz, u);
        
        float vib = std::sin(2.0f * 3.14159265f * params.vibratoRateHz * t) * params.vibratoDepthHz;
        
        // small random walk jitter
        float jit = RandFloatSigned(rng) * params.jitterHz;
        
        float pitchHz = baseHz + vib + jit;
        if (pitchHz < 20.0f) pitchHz = 20.0f;
        if (pitchHz > sr * 0.45f) pitchHz = sr * 0.45f;
        
        // Advance phase
        phase += (pitchHz / sr);
        if (phase >= 1.0f) phase -= (float)((int)phase);
        
        // Voiced source: harmonic-rich “vocal fold” approximation
        // Use a sine harmonic stack with rolloff.
        float src = 0.0f;
        float ph = phase * 2.0f * 3.14159265f;
        
        // Roll-off: higher brightness => slower decay.
        float roll = Lerp(1.35f, 0.70f, brightness);
        
        for (int h = 1; h <= harmonics; ++h) {
            float amp = 1.0f / std::pow((float)h, roll);
            src += amp * std::sin(ph * (float)h);
        }
        
        // Normalize the stack roughly
        src *= 0.65f;
        
        // Add breath noise (smoothed)
        float n = RandFloatSigned(rng);
        noiseLP = noiseLP + noiseA * (n - noiseLP);
        float breath = noiseLP * params.noiseAmount;
        
        float x = src + breath;
        
        // “Growl” roughness: mild nonlinear + subharmonic-ish wobble
        if (params.growl > 0.001f) {
            float growl = Clamp01(params.growl);
            // low-rate AM + soft clipping
            float wob = 1.0f + 0.20f * growl * std::sin(2.0f * 3.14159265f * (18.0f + 8.0f * growl) * t);
            x *= wob;
            x = SoftClip(x * (1.0f + 2.2f * growl));
        }
        
        // Apply formants (sum of resonators)
        float y = 0.0f;
        y += r1.Process(x);
        y += r2.Process(x);
        y += r3.Process(x);
        
        // Envelope
        float env = EnvelopeAHRS(
            t,
            params.attackSeconds,
            params.holdSeconds,
            params.releaseSeconds,
            seconds
        );
        
        y *= env;
        
        // Output gain + clamp to int16-ish range stored in int32
        y *= params.gain;
        
        // Hard clamp
        if (y > 1.0f) y = 1.0f;
        if (y < -1.0f) y = -1.0f;
        
        int32_t out = (int32_t)(y * 32767.0f);
        sample->sampleBuffer.push_back(out);
    }
}
