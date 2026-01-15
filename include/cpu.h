#ifndef CPU_H
#define CPU_H

#include "systemc.h"
#include "audio_frame.h"

SC_MODULE(CPU) {
    // Port definitions for sending and receiving frames to accelerators
    sc_fifo_out<AudioFrame*> to_imdct;
    sc_fifo_in<AudioFrame*> from_filterbank;

    sc_signal<int> frames_decoded;

    // bool flag to select between execution modes
    bool is_cpu_only_mode;

    SC_HAS_PROCESS(CPU);

    // Register the decode_process as a concurrent thread
    CPU(sc_core::sc_module_name name, bool mode) :
        sc_module(name), is_cpu_only_mode(mode){
            SC_THREAD(decode_process);
    }

    // process declarations
    void decode_process();

};

#endif
