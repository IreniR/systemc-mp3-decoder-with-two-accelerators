#ifndef ACC_IMDCT_H
#define ACC_IMDCT_H

#include "systemc.h"
#include "audio_frame.h"
#include "cmath"
#include "audio_processing.h"

SC_MODULE(AcceleratorIMDCT) {

    // Port for receiving and sending audio frame 
    sc_fifo_in<AudioFrame*> in; // From CPU
    sc_fifo_out<AudioFrame*> out; // To Filter Bank Accelerator

    sc_signal<int> frames_processed;
    void process();

    // Register process as concurrent SystemC thread
    SC_CTOR(AcceleratorIMDCT) {
        SC_THREAD(process);
    }

};

#endif
