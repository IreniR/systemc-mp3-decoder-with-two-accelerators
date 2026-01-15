# SystemC MP3 Decoder with Hardware Acceleration  

## Project Overview
This project is a functional model of an MP3 Decoder implemented using **SystemC TLM (Transaction Level Modeling)**[cite: 1, 22]. The primary goal is to demonstrate the performance benefits of **hardware-software partitioning** by moving computationally expensive decoding stages—specifically the **IMDCT** and **Synthesis Filter Bank**—from a general-purpose CPU to dedicated hardware accelerators[cite: 15, 98, 133].

The system models two operational scenarios:
1.  **Sequential Baseline (CPU-Only):** All decoding stages are executed on a single CPU thread[cite: 88].
2.  **Concurrent Accelerated Run:** The bottleneck stages are offloaded to concurrent hardware modules, allowing for pipelined execution[cite: 98, 99].

## System Architecture
The decoder is modeled as a 3-module pipeline connected via **SystemC TLM FIFOs** (`sc_fifo`)[cite: 22, 60].


### Modules & Pipeline Stages
| Stage | Process | Partitioning | Latency | Role |
| :--- | :--- | :--- | :--- | :--- |
| 1 & 2 | Huffman Decode & Dequantize | CPU | 10 ns | Initial light processing [cite: 38-42]. |
| 3 | IMDCT | Accelerator | 50 ns | Heavy computational load [cite: 43-47]. |
| 4 | Synthesis Filter Bank | Accelerator | 80 ns | System's critical bottleneck [cite: 48-52]. |

**Data Flow:**
* **CPU:** Decodes frequency coefficients and sends them to the IMDCT Accelerator[cite: 62, 83].
* **IMDCT Accelerator:** Processes coefficients into time-domain samples and forwards them to the Filter Bank[cite: 63, 84].
* **Filter Bank Accelerator:** Performs the final synthesis and returns the PCM output back to the CPU[cite: 64, 85].

## Performance Results
Based on a simulation of 10 audio frames, the hardware acceleration provides a significant throughput improvement[cite: 81, 130]:

| Metric | Sequential (Baseline) | Concurrent (Accelerated) |
| :--- | :--- | :--- |
| **Throughput (per frame)** | 140 ns | 80 ns |
| **Total Simulation Time** | 1400 ns | 860 ns |
| **System Speedup** | 1.0x | **1.63x** |

The system's throughput in accelerated mode is limited only by the slowest stage (the 80 ns Filter Bank), demonstrating the effectiveness of the concurrent pipeline[cite: 100, 103].

## File Structure
* `cpu.h` / `.cpp`: Implements the CPU module with Huffman decoding and dequantization.
* `accelerator_imdct.h` / `.cpp`: Implements the IMDCT hardware accelerator thread.
* `accelerator_filterbank.h` / `.cpp`: Implements the Synthesis Filter Bank hardware accelerator thread.
* `audio_processing.h`: Contains the mathematical functions for decoding.
* `audio_frame.h`: Defines the `AudioFrame` data structure passed through FIFOs.
* `timing.h`: Centralizes latency constants (e.g., `CPU_PROC_TIME`, `IMDCT_PROC_TIME`).
* `main.cpp`: Orchestrates the simulation, instantiates modules, and binds FIFO channels.
* `Makefile`: Build script for the project.

## Requirements
* **SystemC Library** (e.g., SystemC 2.3.x or 3.0.0).
* **C++17** compatible compiler (GCC/Clang).

## How to Build and Run
1.  **Setup Environment:** Ensure the `SYSTEMC_HOME` variable in the `Makefile` points to your SystemC installation path.
2.  **Compile:**
    ```bash
    make
    ```
3.  **Run Simulation:**
    ```bash
    ./mp3_decoder
    ```
4.  **Configuration:** You can toggle between Sequential and Accelerated modes by changing the `CPU_ONLY_MODE` boolean in `main.cpp`.
5.  **Waveform Analysis:** The simulation generates a `trace.vcd` file which can be viewed in tools like **GTKWave** to verify timing and frame synchronization.