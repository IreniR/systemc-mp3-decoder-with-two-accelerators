#include "accelerator_imdct.h"
#include <vector>
#include <iostream>
#include <cmath>
#include "audio_frame.h"
#include "systemc.h"
#include "audio_processing.h"
#include "timing.h"

using namespace sc_core;

void AcceleratorIMDCT::process() {
    while(true) {
        // Blocking read stops thread until audio frame is available in input fifo
        AudioFrame* frame = in.read();

        // Simulates time required for IMDCT HW to execute
        wait(IMDCT_PROC_TIME);
        performIMDCT(*frame);
        frames_processed.write(frame->id);
        std::cout << sc_time_stamp() << ": IMDCT processed frame " << frame->id << "\n";
        
        // Send processed frame to output fifo
        out.write(frame);
    }
}
