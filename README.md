# Battery Management System (BMS) Simulator in C

A modular, C-based Battery Management System simulator designed to monitor battery parameters, detect abnormal operating conditions, implement cell balancing, and simulate battery protection mechanisms.

The project follows an embedded-style software architecture with hardware abstraction, periodic task scheduling, state-machine-based control, and diagnostic logging.

## Project Overview

This project simulates a 4-cell battery pack and demonstrates the fundamental software functionalities of a Battery Management System (BMS).

It monitors individual cell voltages, battery current, temperature, and State of Charge (SOC). Based on simulated sensor readings, the system identifies abnormal conditions and initiates appropriate warning, fault, and shutdown responses.

## Features

- **Battery Monitoring:** Monitors individual cell voltages, pack voltage, current, temperature, and State of Charge (SOC).
- **Battery Parameter Calculation:** Calculates average, minimum, maximum, and voltage difference between cells, along with battery power.
- **Fault Detection:** Identifies overvoltage, undervoltage, overcurrent, overheating, and cell imbalance conditions.
- **Cell Balancing:** Implements voltage-based cell balancing logic to reduce voltage differences between cells.
- **State Machine:** Manages system operating states, including IDLE, MONITORING, WARNING, FAULT, and SHUTDOWN.
- **Battery Protection:** Simulates protective shutdown and battery disconnection during critical fault conditions.
- **Hardware Abstraction Layer (HAL):** Separates simulated sensor inputs and hardware-related operations from application logic.
- **Periodic Task Scheduling:** Organizes battery monitoring and protection operations through scheduled software tasks.
- **Diagnostic Logging:** Records system events, state transitions, and battery parameters.
- **Fault History:** Maintains a history of detected faults, including severity and occurrence time.
- **Performance Measurement:** Measures CPU execution time using the C standard library.

## Project Structure

```text
Battery-Management-System/
│
├── src/
│   ├── main.c
│   ├── battery.c
│   ├── battery.h
│   ├── balancing.c
│   ├── balancing.h
│   ├── config.h
│   ├── fault.c
│   ├── fault.h
│   ├── hal.c
│   ├── hal.h
│   ├── logger.c
│   ├── logger.h
│   ├── scheduler.c
│   ├── scheduler.h
│   ├── state_machine.c
│   └── state_machine.h
│
├── tests/
│   └── test_bms.c
│
├── Makefile
├── README.md
└── .gitignore
```

## System Architecture

The project is organized into independent software modules to improve readability, maintainability, and scalability.

| Module | Responsibility |
|---|---|
| `main.c` | Main simulation loop and module integration |
| `battery.c/.h` | Battery parameter calculations and SOC estimation |
| `balancing.c/.h` | Cell voltage balancing logic |
| `fault.c/.h` | Fault detection, severity classification, and fault history |
| `hal.c/.h` | Simulated sensor inputs and hardware abstraction |
| `logger.c/.h` | Diagnostic and event logging |
| `scheduler.c/.h` | Periodic task scheduling |
| `state_machine.c/.h` | Battery operating states and transitions |
| `config.h` | Battery configuration and operating thresholds |

## Technologies Used

- **Programming Language:** C
- **Compiler:** GCC
- **Build System:** GNU Make
- **Development Environment:** Visual Studio Code
- **Platform:** Windows with MSYS2 UCRT64
- **Testing:** Custom C-based automated test suite

## Build and Execution

### Prerequisites

Install the following tools:

- GCC compiler
- GNU Make
- Git (optional, for cloning the repository)

### Clone the Repository

```bash
git clone https://github.com/mkkaavya/Battery-Management-System.git
cd Battery-Management-System
```

### Compile the Project

```bash
make
```

### Run the Simulation

```bash
./bms
```

### Run Automated Tests

```bash
make test
```

## Test Results

The project includes an automated test suite to validate battery calculations, fault detection, cell balancing, state transitions, and other implemented functionalities.

**Latest test execution:**

```text
========== BMS TEST REPORT ==========
Tests passed: 50
Tests failed: 0
Total tests:  50
=====================================
```

All 50 tests passed during the latest recorded test execution.

## Sample Simulation Output

```text
[00:00.000] Battery monitoring started
[00:00.100] CELL IMBALANCE WARNING DETECTED
[00:00.100] State: MONITORING -> WARNING

[00:07.100] State: WARNING -> MONITORING

[00:16.000] OVERHEATING WARNING DETECTED
[00:18.000] OVERHEATING FAULT DETECTED
[00:18.200] State: FAULT -> SHUTDOWN

[00:18.200] SYSTEM SHUTDOWN - pack disconnected

[00:30.000] Simulation finished
```

## Future Improvements

- Sensor failure and invalid sensor reading detection.
- Fault recovery and system reinitialization mechanisms.
- Handling multiple simultaneous fault conditions.
- Expanded boundary and stress testing.
- Hardware integration with microcontrollers and real battery sensors.
- Communication interfaces such as CAN for battery monitoring.
- Integration with an RTOS for more realistic embedded task scheduling.

## Limitations

This project is a software-based battery management simulation. Sensor readings and hardware operations are simulated through software abstractions.

It does not currently implement physical battery monitoring, real cell balancing hardware, or safety-certified battery protection mechanisms.


---

*Developed as an embedded C project to explore battery monitoring, fault management, cell balancing, and modular firmware architecture.*