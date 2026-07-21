#ifndef SAMPLE_PARAMETERS_H
#define SAMPLE_PARAMETERS_H

#include <stdint.h>

struct VocalParameters {
    // Duration
    float seconds = 0.45f;
    
    // Pitch contour (Hz)
    float pitchStartHz = 220.0f;
    float pitchEndHz   = 140.0f;
    
    // Vibrato
    float vibratoRateHz  = 6.0f;
    float vibratoDepthHz = 8.0f;
    
    // Random pitch jitter (Hz)
    float jitterHz = 2.0f;
    
    // Source timbre
    float brightness = 0.65f; // 0..1 (higher => more harmonics)
    float growl      = 0.25f; // 0..1 (nonlinear distortion / roughness)
    
    // Breath / turbulence noise (0..1)
    float noiseAmount = 0.12f;
    
    // Envelope
    float attackSeconds  = 0.015f;
    float holdSeconds    = 0.10f;
    float releaseSeconds = 0.25f;
    
    // Simple formant model (Hz + bandwidth Hz)
    float formant1Hz = 600.0f;
    float formant2Hz = 1400.0f;
    float formant3Hz = 2500.0f;
    
    float bandwidth1Hz = 120.0f;
    float bandwidth2Hz = 180.0f;
    float bandwidth3Hz = 260.0f;
    
    // Overall output gain (0..1-ish)
    float gain = 0.9f;
    
    // Seed for deterministic variation (0 = random)
    uint32_t seed = 0;
};

#endif
