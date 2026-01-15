#include "cpu.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include "audio_frame.h"
#include "systemc.h"
#include "audio_processing.h"
#include "random"
#include "timing.h"

using namespace sc_core;

// External declarations for global time constants
extern sc_time CPU_PROC_TIME;
extern sc_time TOTAL_SEQ_TIME;

// Mimic functional output of two stages
void huffmanDecode(AudioFrame &frame, int numCoeffs, std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    frame.frequencyCoeffs_L.clear();
    frame.frequencyCoeffs_R.clear();
    frame.frequencyCoeffs_L.reserve(numCoeffs);
    frame.frequencyCoeffs_R.reserve(numCoeffs);

    // Frequency coefficients
    // Populate frame with initial frequency coefficients, to represent data decoded successfully
    for(int i=0;i<numCoeffs;i++) {
        frame.frequencyCoeffs_L.push_back(dist(rng));
        frame.frequencyCoeffs_R.push_back(dist(rng));
    }
}

// Mimic complex mathematical conversion of quantized coefficients back to scaled floating point values
void dequantize(AudioFrame &frame) {
    // Apply non linear dequantization formula
    // Simulates data transformation to left and right channel
    for(auto &coeff : frame.frequencyCoeffs_L) {
        coeff = std::copysign(std::pow(std::abs(coeff), 4.0/3.0), coeff); 
    }

    for(auto &coeff : frame.frequencyCoeffs_R) {
        coeff = std::copysign(std::pow(std::abs(coeff), 4.0/3.0), coeff); 
    }
}

void CPU::decode_process() {
    const int totalFrames = 10;
    const int numCoeffs = 64; // number of frequency-domain coefficients per frame

    std::mt19937 rng(1234);

    if(is_cpu_only_mode) {
        std::cout << "SEQ CPU-ONLY RUN\n";
        std::cout << "Total time per frame: " << TOTAL_SEQ_TIME << "\n";

          for(int i=0;i<totalFrames;i++) {
            AudioFrame* frame = new AudioFrame();
            frame->id = i;

            // Huffman decoding
            huffmanDecode(*frame, numCoeffs, rng);

            // Dequantization
            dequantize(*frame);
            performIMDCT(*frame);
            performFilterBank(*frame);
            frames_decoded.write(i);

            wait(TOTAL_SEQ_TIME);
            
            std::cout << sc_time_stamp() << ": CPU decoded frame " << i << "\n";
            delete frame;

        }
    } else{
        std::cout << "CPU-ACCELERATOR RUN\n";

        for(int i=0;i<totalFrames;i++) {
            AudioFrame* frame = new AudioFrame();
            frame->id = i;

            // Huffman decoding
            huffmanDecode(*frame, numCoeffs, rng);

            // Dequantization
            dequantize(*frame);

            wait(CPU_PROC_TIME);
            frames_decoded.write(i);
            std::cout << sc_time_stamp() << ": CPU decoded frame " << i << "\n";

            // Send to IMDCT accelerator
            to_imdct.write(frame);
        }
        for(int i =0; i < totalFrames; i++){
            // Receive processed frame from filter bank
            AudioFrame* processed = from_filterbank.read();
            std::cout << sc_time_stamp() << ": CPU received processed frame " << processed->id << "\n";

            delete processed;
        }
    }

    sc_stop();
}
