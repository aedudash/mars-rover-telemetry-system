# Mars Rover Telemetry & Reasoning System

A semester-long C++ software engineering project that models the telemetry-processing and reasoning pipeline of a remotely deployed scientific rover.

The system was developed incrementally across four project phases. It grew from foundational measurement and unit-handling components into a modular application capable of processing heterogeneous rover telemetry, calculating navigation and environmental statistics, interpreting spectral observations, reconciling logical propositions, and evaluating higher-level arguments using three-valued logic.

## Project Context

The project models a scientific robot operating in a remote environment such as another planet or an inaccessible terrestrial location.

Rather than receiving clean application-level data, the rover processes intentionally minimal telemetry representing information that could be transmitted from navigation, environmental, and scientific sensors during deployment.

The mission data intentionally includes challenges such as:

- Mixed measurement units
- Sparse or incomplete sensor records
- Malformed or invalid records
- Multiple sensor types within a common data stream
- Limited contextual information
- Missing or conflicting logical assertions
- Records that must be discarded without terminating the overall mission-processing pipeline

A major design objective was therefore to distinguish between recoverable telemetry problems and failures that prevent continued processing.

The result is a system designed to extract useful mission information while continuing operation through imperfect input.

## System Pipeline

The completed application follows this approximate processing pipeline:

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
```

## Major Components

### Unit and Measurement System

`Unit` and `Measurement` provide a reusable dimensional measurement layer used throughout the rover-processing pipeline.

The system supports:

- Seven SI base dimensions
- Unit aliases and normalization
- Length and time conversions
- Martian SOL time representation
- Celsius, Kelvin, and Fahrenheit definitions
- Canonical base-unit conversion
- Dimension-checked addition and subtraction
- Dimension composition during multiplication and division
- Dimension-checked comparisons

This allows telemetry to be processed consistently even when different records use different units.

### Rover Integration

`Robot` serves as the primary integration layer for incoming rover data.

It reads the CSV-style telemetry stream line-by-line and routes records to the appropriate subsystem based on the record type.

Supported telemetry categories include:

```text
t   Temperature measurement
w   Spectral wavelength / intensity sample
d   Rover displacement information
```

The integration layer also tolerates recoverable parsing and unit errors so that a malformed record does not automatically terminate processing of later mission data.

### Navigation

The navigation subsystem converts rover movements into two-dimensional displacement vectors.

Movement distances are normalized to base units and projected into X/Y components according to direction.

At the end of each SOL, the system calculates:

- Net displacement magnitude
- Resulting heading
- Total recorded travel time

The completed project also calculates maximum displacement values for sequential 100-SOL mission blocks and sorts those maxima for reporting.

### Temperature Statistics

The temperature-processing subsystem maintains mission-level environmental statistics.

It calculates:

- Arithmetic mean
- Median
- Three highest recorded temperatures
- Three lowest recorded temperatures
- Lowest Summer temperature
- Lowest Winter temperature

The median calculation uses two priority queues:

```text
Lower half -> max heap
Upper half -> min heap
```

This allows the median to be maintained incrementally as telemetry arrives.

### Spectral Samples

Each `Sample` stores:

- Wavelength
- Intensity
- SOL index

Wavelength measurements are normalized into base units before classification.

The current academic implementation groups wavelengths into coarse spectral regions:

```text
UV
Visible
Infrared
```

These classifications are abstractions used by the course project rather than scientific material-identification claims.

### SHERLOC Classifier

`SherlocClassifier` aggregates spectral classifications and records the first SOL in which each unique classification appears.

Results can be returned in either chronological or reverse encounter order.

### Planet Model

`Planet` provides environmental constants used by the rover.

The Mars configuration includes:

- SOL duration
- Martian year length
- Seasonal lookup based on SOL index

This allows temperature measurements to be associated with approximate Martian seasons.

## Logical Reasoning System

The later project phases extend the telemetry-processing system with a separate symbolic reasoning layer.

### Proposition Model

`Proposition` represents structured logical claims.

Each proposition contains:

- Label
- Predicate form
- Subject
- Parameter type
- Numeric value
- Negation state

Examples include:

```text
detected(water,500)
all(hydrogen,1000)
average(temperature,30c)
not(...)
```

Propositions can be compared for structural equivalence or direct logical negation.

### MSL Assertion Loader

`MSLAssertionLoader` parses externally supplied MSL assertions into structured `Proposition` objects.

The parser handles:

- Whitespace normalization
- Case normalization
- Negation
- Numeric parameters
- Temperature conversion
- Repeated proposition labels
- Incomplete proposition sets

### Perseverance Proposition Builder

`PerseveranceBuilder` derives a corresponding set of propositions from the rover's processed telemetry.

This layer connects numeric rover observations to the symbolic reasoning system.

The academic implementation derives propositions P through T from combinations of:

- Temperature statistics
- Spectral intensity
- Navigation distance

### Truth Reconciliation

`TruthReconciler` compares the MSL proposition set against propositions derived from Perseverance telemetry.

The reconciled truth mapping uses three possible states:

```text
T   True
F   False
?   Unknown
```

`Unknown` allows the system to represent missing, conflicting, or structurally incompatible information rather than forcing every result into a binary state.

### Ratiocinator

The `Ratiocinator` evaluates higher-level logical arguments using the reconciled truth values.

Supported operators are:

```text
~    NOT
*    AND
+    OR
:-   IMPLIES
```

Expressions are processed in three stages:

1. Tokenization
2. Infix-to-postfix conversion using a stack-based shunting-yard-style algorithm
3. Postfix evaluation using three-valued logical operations

Example argument definitions:

```text
Y = R * T + P
X = Q * R :- S
Z = ~S * P
```

Operator precedence is handled by the parser rather than requiring every expression to be fully parenthesized.

## Fault-Tolerant Telemetry Processing

A central requirement of the project was continued processing in the presence of imperfect mission data.

The telemetry dataset intentionally includes malformed or incomplete records designed to represent communication or sensor-data problems that could occur on a remotely deployed platform.

When possible, the software discards the affected record or record group and continues processing later telemetry.

This design prevents a single recoverable measurement error from invalidating the remainder of the mission dataset.

## Repository Structure

```text
mars-rover-telemetry-system/
|
|-- CMakeLists.txt
|-- README.md
|-- .gitignore
|
|-- src/
|   |-- main.cpp
|   |-- Measurement.cpp
|   |-- MSLAssertionLoader.cpp
|   |-- Navigation.cpp
|   |-- PerseveranceBuilder.cpp
|   |-- Planet.cpp
|   |-- Proposition.cpp
|   |-- Ratiocinator.cpp
|   |-- Robot.cpp
|   |-- Sample.cpp
|   |-- SherlocClassifier.cpp
|   |-- TemperatureStats.cpp
|   |-- TruthReconciler.cpp
|   `-- Unit.cpp
|
|-- include/
|   `-- C++ header files
|
|-- data/
|   |-- mission/
|   |   |-- argument.txt
|   |   |-- lab3Data.txt
|   |   `-- msl_assertion.txt
|   |
|   `-- diagnostics/
|       `-- development and troubleshooting datasets
|
|-- output/
|   `-- expected_mission_report.txt
|
`-- docs/
    `-- original development documentation
```

## Build and Run

### Requirements

- CMake 3.16 or newer
- A C++11-compatible compiler

The Windows build has been verified using:

- Windows 10
- Visual Studio 2022 Build Tools
- MSVC
- CMake

For Visual Studio Build Tools, install the **Desktop development with C++** workload.

### Build on Windows

From the repository root:

```cmd
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The resulting executable is generated at:

```text
build\Release\rover.exe
```

### Run the Mission Dataset

From the repository root:

```cmd
build\Release\rover.exe data\mission\msl_assertion.txt data\mission\argument.txt data\mission\lab3Data.txt
```

The program generates:

```text
mission_report.txt
```

The generated report contains:

- Temperature statistics
- Spectral classification results
- Navigation results
- 100-SOL maximum displacement analysis
- MSL and Perseverance propositions
- Reconciled P-T truth values
- Evaluated X/Y/Z logical arguments

### Verify the Output

A verified reference report is included at:

```text
output\expected_mission_report.txt
```

On Windows, the generated report can be compared against the reference output with:

```cmd
fc mission_report.txt output\expected_mission_report.txt
```

A matching run should return:

```text
FC: no differences encountered
```

## Command-Line Interface

General invocation:

```text
rover <msl_assertion.txt> <argument.txt> <data1> [data2 ...]
```

The application can process one or more rover telemetry files during a single execution.

Each telemetry dataset is processed as an independent section of the final mission report.

## Example Mission Output

A completed run includes sections similar to:

```text
============================================================
Report for input file: lab3Data.txt
============================================================

-- Temperature --
mean C: ...
mean K: ...
median C: ...
median K: ...

-- Unique substances by wavelength (chronological) --
...

-- Navigation --
...

-- 100-SOL block max distances (sorted descending) --
...

-- Propositions P-T (MSL vs Perseverance) --
Label P:
  MSL: ...
  PER: ...
  tau(P) = ...

-- Arguments X, Y, Z --
...
```

The term `substances` appears in the original academic report format. In the current implementation, the classifier represents coarse wavelength regions rather than scientifically validated material identification.

## Development Process

The project was developed throughout a semester as a sequence of four expanding assignments.

Each phase extended the previous implementation rather than replacing it, resulting in a progressively larger system containing:

- Dimensional unit representation
- Measurement conversion and validation
- Statistical processing
- Planetary and seasonal modeling
- Navigation and vector calculations
- Spectral telemetry
- Rover subsystem integration
- Structured proposition parsing
- Evidence reconciliation
- Three-valued logic
- Expression parsing
- Mission-report generation

The repository therefore reflects an iterative engineering process rather than a single isolated programming assignment.

## Engineering Documentation

The original project documentation is preserved in the `docs/` directory.

It includes design artifacts such as:

- Input-Process-Output models
- Finite state tables
- Finite state diagrams
- Algebraic specifications
- State-machine models
- Use-case diagrams
- UML class diagrams
- Implementation notes
- Example application output

Some documentation reflects an earlier stage of the evolving implementation and may differ slightly from the final Project 4 source code.

## Original Course Version

The recovered final academic version of the project is preserved in the Git tag/release:

```text
course-final-2025
```

This preserves the original project state prior to portfolio cleanup and modernization.

The `main` branch contains subsequent repository organization, documentation improvements, build-system integration, portability improvements, and verified output formatting.

## Portfolio Modernization

The original academic implementation has intentionally not been rewritten from scratch.

The modernization effort focuses on preserving the original engineering work while making the repository easier to build, inspect, verify, and discuss professionally.

Completed improvements include:

- Original course version preserved through Git history and release tagging
- Source and header files reorganized into dedicated directories
- Mission and diagnostic data separated
- CMake build system added
- Windows/MSVC build verified
- Generated build artifacts excluded through `.gitignore`
- Mission report naming standardized
- Terminal-dependent Unicode output replaced with portable ASCII
- Machine-specific input paths removed from generated report headers
- Verified reference mission output preserved for regression comparison

Additional testing and implementation cleanup may be added in later revisions.

## Future Development

A future extension of the project may adapt portions of this architecture to a physical 3D-printed rover.

Potential development areas include:

- Embedded motor control
- Wheel odometry
- Environmental sensors
- Real sensor telemetry
- Wireless communication
- Material or terrain classification
- Autonomous navigation
- Microcontroller integration
- Integration of physical sensor observations with the existing reasoning architecture

The long-term goal is to preserve the lineage of the original software project while transitioning from simulated mission telemetry to physical robotic hardware.

## Technologies and Concepts

```text
C++
CMake
Object-Oriented Design
Dimensional Analysis
Telemetry Parsing
Fault-Tolerant Input Processing
Data Structures
Priority Queues
Vector Mathematics
Streaming Statistics
Finite State Modeling
Symbolic Logic
Three-Valued Logic
Expression Parsing
Infix-to-Postfix Conversion
Stack Evaluation
File I/O
Git / GitHub
```

## Status

The original academic project is complete.

The current `main` branch is a portfolio-focused modernization of that completed implementation.