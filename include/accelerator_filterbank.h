#ifndef ACC_FILTERBANK_H
#define ACC_FILTERBANK_H

#include "systemc.h"
#include "audio_frame.h"  

SC_MODULE(AcceleratorFilterBank) {
    // Ports for sending and receiving audio frame
    sc_fifo_in<AudioFrame*> in; // From IMDCT Accelerator
    sc_fifo_out<AudioFrame*> out; // To CPU

    sc_signal<int> frames_processed;

    void process();

    SC_CTOR(AcceleratorFilterBank) {
        SC_THREAD(process);
    }
};

#endif
