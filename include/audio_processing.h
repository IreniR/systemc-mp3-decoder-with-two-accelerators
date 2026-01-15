#pragma once
#include <vector>
#include <cmath>
#include <numeric>
#include "audio_frame.h"

// Calculate PI for trigonometric calculations
inline double PI() { return std::acos(-1.0); }

// Perform simple IMDCT computation for single channel
inline void imdct_type_iv_single(const std::vector<float>& X, std::vector<float>& x_out) {
    int N = (int)X.size(); // N for number of input frequency coefficients
    if (N == 0) { x_out.clear(); return; }

    x_out.assign(N, 0.0f);
    double pi = PI();
    for (int n = 0; n < N; ++n) {
        double sum = 0.0;
        for (int k = 0; k < N; ++k) {
            // Calculate angle for cosine basis function
            double angle = pi / N * ( (double)n + 0.5 + N/2.0 ) * ( (double)k + 0.5 );
            // Accumulates weighted sum
            sum += X[k] * std::cos(angle);
        }
        // Assign final time-domain sample
        x_out[n] = (float)sum;
    }
}

// Applies complex IMDCT transformations to left and right channel
inline void performIMDCT(AudioFrame& frame) {
    imdct_type_iv_single(frame.frequencyCoeffs_L, frame.samples_L);
    imdct_type_iv_single(frame.frequencyCoeffs_R, frame.samples_R);
}

// Create prototype FIR filter
inline std::vector<float> make_prototype_fir(int taps, float cutoff = 0.5f) {
    // Generate filter coefficients using windowed sinc function
    // Adds computational weight to filter bank stage
    std::vector<float> h(taps);
    int M = taps - 1;
    double fc = cutoff; 
    double pi = PI();
    for (int n = 0; n <= M; ++n) {
        double m = n - M / 2.0;
        double ifs = (m == 0.0) ? 2.0 * fc : std::sin(2.0 * pi * fc * m) / (pi * m);
        double w = 0.5 * (1.0 - std::cos(2.0 * pi * n / M)); // Windowing function
        h[n] = (float)(ifs * w); 
    }
    // Normalization
    float sum = std::accumulate(h.begin(), h.end(), 0.0f);
    if (sum != 0.0f) {
        for (auto &v : h) v /= sum;
    }
    return h;
}

// Simplified convolutional-based synthesis filter bank operation
inline void synthesis_filterbank_simple(const std::vector<float>& in_samples, std::vector<float>& out_pcm, 
                                       const std::vector<float>& prototype, int num_subbands = 8)
{
    if (in_samples.empty()) { out_pcm.clear(); return; }

    int taps = (int)prototype.size();
    int inN = (int)in_samples.size();

    // Convolution filtering
    out_pcm.assign(inN, 0.0f);
    for (int n = 0; n < inN; ++n) {
        double acc = 0.0;
        for (int k = 0; k < taps; ++k) {
            // Convolution sum
            int idx = n - k;
            if (idx >= 0 && idx < inN) acc += in_samples[idx] * prototype[k];
        }
        out_pcm[n] = (float)acc;
    }
    // Simple Downsampling
    if (num_subbands <= 1) return;
    std::vector<float> down;
    int outLen = (inN + num_subbands - 1) / num_subbands;
    down.assign(outLen, 0.0f);
    for (int i = 0; i < inN; ++i) {
        down[i / num_subbands] += out_pcm[i];
    }
    out_pcm.swap(down);
}

inline void performFilterBank(AudioFrame& frame) {
    static std::vector<float> prototype = make_prototype_fir(16, 0.45f);
    const int SUBBANDS = 8;

    std::vector<float> outL, outR;
    // Process time domain samples from IMDCT through simulated filter bank
    synthesis_filterbank_simple(frame.samples_L, outL, prototype, SUBBANDS);
    synthesis_filterbank_simple(frame.samples_R, outR, prototype, SUBBANDS);

    // Update frame with final PCM output samples
    frame.samples_L.swap(outL);
    frame.samples_R.swap(outR);
}
