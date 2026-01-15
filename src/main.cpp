#include "systemc.h"
#include "cpu.h"
#include "accelerator_imdct.h"
#include "accelerator_filterbank.h"
#include <fstream>

const bool CPU_ONLY_MODE = false;

sc_time CPU_PROC_TIME;
sc_time IMDCT_PROC_TIME;
sc_time FILTERBANK_PROC_TIME;
sc_time TOTAL_SEQ_TIME;

int sc_main(int argc, char* argv[]) {

    // Time resolution for entie simulations
    sc_set_time_resolution(1, SC_NS);

    // Timing parameters
    CPU_PROC_TIME = sc_time(10, SC_NS);
    IMDCT_PROC_TIME = sc_time(50, SC_NS);
    FILTERBANK_PROC_TIME = sc_time(80, SC_NS);
    TOTAL_SEQ_TIME = CPU_PROC_TIME + IMDCT_PROC_TIME + FILTERBANK_PROC_TIME;

    // Creating to and from channels
    sc_core::sc_fifo<AudioFrame*> fifoCPU_IMDCT(10);
    sc_core::sc_fifo<AudioFrame*> fifoIMDCT_Filter(10);
    sc_core::sc_fifo<AudioFrame*> fifoFilter_CPU(10);

    CPU cpu("CPU", CPU_ONLY_MODE);

    // CPU bindings
    AcceleratorIMDCT imdct("IMDCT");
    AcceleratorFilterBank filter("FilterBank");

    cpu.to_imdct(fifoCPU_IMDCT);
    cpu.from_filterbank(fifoFilter_CPU);

    // IMDCT accelerator bindings
    imdct.in(fifoCPU_IMDCT);
    imdct.out(fifoIMDCT_Filter);

    // Filter bank accelerator bindings
    filter.in(fifoIMDCT_Filter);
    filter.out(fifoFilter_CPU);

    sc_trace_file *tf = sc_create_vcd_trace_file("trace");
    sc_trace(tf, cpu.frames_decoded, "CPU_frames_decoded");
    sc_trace(tf, imdct.frames_processed, "IMDCT_frames_decoded");
    sc_trace(tf, filter.frames_processed, "FILTER_frames_decoded");

    if (CPU_ONLY_MODE){
        std::cout<<"Running in CPU-Only Mode\n";
    }else{
         std::cout<<"Running in CPU-w/-Accelerators Mode\n";
    }

    // start simulation loop
    sc_core::sc_start();
    sc_close_vcd_trace_file(tf);

    return 0;
}
