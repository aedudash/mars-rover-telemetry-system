# Mars Rover Telemetry & Reasoning System

A semester-long C++ software engineering project that models the telemetry-processing pipeline of a remotely deployed scientific rover.

The system was developed incrementally across four project phases. It grew from foundational measurement and unit-handling components into a modular application capable of processing rover telemetry, calculating navigation and environmental statistics, interpreting spectral observations, reconciling logical propositions, and evaluating higher-level arguments using three-valued logic.

## Project Context

The rover receives intentionally minimal and imperfect telemetry representing data that might be transmitted by a remote scientific platform deployed in an environment such as another planet or an inaccessible terrestrial location.

The supplied telemetry was intentionally designed to contain challenges such as:

- Mixed measurement units
- Sparse or incomplete sensor records
- Malformed or invalid records
- Multiple sensor data types in a common stream
- Limited contextual information
- Records that must be skipped without terminating the mission-processing pipeline

The program therefore distinguishes between recoverable data problems and failures that prevent continued processing.

## System Pipeline

The final system follows an approximate processing pipeline:

```text
Rover Telemetry
      |
      v
Unit / Measurement Normalization
      |
      v
+-----------------------------+
| Rover Processing Subsystems |
|                             |
| Navigation                  |
| Temperature Statistics      |
| Spectral Samples            |
| SHERLOC Classification      |
+-----------------------------+
      |
      v
Perseverance Proposition Builder
      |
      +------------------+
      |                  |
      v                  v
Rover Propositions    MSL Assertions
      |                  |
      +--------+---------+
               |
               v
        Truth Reconciliation
               |
               v
        P-Q-R-S-T Truth Map
               |
               v
          Ratiocinator
               |
               v
     X-Y-Z Argument Evaluation
               |
               v
        Final Mission Report
