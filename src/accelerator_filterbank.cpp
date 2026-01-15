#include "accelerator_filterbank.h"
#include <vector>
#include <iostream>
#include "audio_frame.h"
#include "systemc.h"
#include "timing.h"
#include "audio_processing.h"

using namespace sc_core;

void AcceleratorFilterBank::process() {
    while(true) {
        // Stops thread until audio frame is available in input fifo
        AudioFrame* frame = in.read();

        performFilterBank(*frame);

        // Simulates time required for filter Bank HW
        wait(FILTERBANK_PROC_TIME);
        frames_processed.write(frame->id);
        std::cout << sc_time_stamp() << ": Filter Bank processed frame " << frame->id << "\n";
        out.write(frame);
    }
}
