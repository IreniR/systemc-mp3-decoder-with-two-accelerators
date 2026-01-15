# SystemC MP3 Decoder with Hardware Acceleration  

## Project Overview
[cite_start]This project is a functional model of an MP3 Decoder implemented using **SystemC TLM (Transaction Level Modeling)**[cite: 1, 22]. [cite_start]The primary goal is to demonstrate the performance benefits of **hardware-software partitioning** by moving computationally expensive decoding stages—specifically the **IMDCT** and **Synthesis Filter Bank**—from a general-purpose CPU to dedicated hardware accelerators[cite: 15, 98, 133].

The system models two operational scenarios:
1.  [cite_start]**Sequential Baseline (CPU-Only):** All decoding stages are executed on a single CPU thread[cite: 88].
2.  [cite_start]**Concurrent Accelerated Run:** The bottleneck stages are offloaded to concurrent hardware modules, allowing for pipelined execution[cite: 98, 99].

## System Architecture
[cite_start]The decoder is modeled as a 3-module pipeline connected via **SystemC TLM FIFOs** (`sc_fifo`)[cite: 22, 60].


### Modules & Pipeline Stages
| Stage | Process | Partitioning | Latency | Role |
| :--- | :--- | :--- | :--- | :--- |
| 1 & 2 | Huffman Decode & Dequantize | CPU | 10 ns | [cite_start]Initial light processing [cite: 38-42]. |
| 3 | IMDCT | Accelerator | 50 ns | [cite_start]Heavy computational load [cite: 43-47]. |
| 4 | Synthesis Filter Bank | Accelerator | 80 ns | [cite_start]System's critical bottleneck [cite: 48-52]. |

**Data Flow:**
* [cite_start]**CPU:** Decodes frequency coefficients and sends them to the IMDCT Accelerator[cite: 62, 83].
* [cite_start]**IMDCT Accelerator:** Processes coefficients into time-domain samples and forwards them to the Filter Bank[cite: 63, 84].
* [cite_start]**Filter Bank Accelerator:** Performs the final synthesis and returns the PCM output back to the CPU[cite: 64, 85].

## Performance Results
[cite_start]Based on a simulation of 10 audio frames, the hardware acceleration provides a significant throughput improvement[cite: 81, 130]:

| Metric | Sequential (Baseline) | Concurrent (Accelerated) |
| :--- | :--- | :--- |
| **Throughput (per frame)** | [cite_start]140 ns [cite: 116] | [cite_start]80 ns [cite: 117] |
| **Total Simulation Time** | [cite_start]1400 ns [cite: 119] | [cite_start]860 ns [cite: 120] |
| **System Speedup** | 1.0x | [cite_start]**1.63x** [cite: 123, 126] |

[cite_start]The system's throughput in accelerated mode is limited only by the slowest stage (the 80 ns Filter Bank), demonstrating the effectiveness of the concurrent pipeline[cite: 100, 103].

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