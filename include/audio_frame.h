#pragma once
#include <vector>

struct AudioFrame {
    std::vector<float> samples_L;  // PCM samples or frequency coefficients
    std::vector<float> frequencyCoeffs_L;

    // Right Channel
    std::vector<float> samples_R;  // PCM samples or frequency coefficients
    std::vector<float> frequencyCoeffs_R;

    int id = -1;                      // frame identifier
};
