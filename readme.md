# Vehicle Control Unit - Zips Racing 2025
## Usage
For help on how to setup this project, see the below file:

[Firmware Toolchain Setup Guide (common/doc/firmware_toolchain_guide.md)](common/doc/firmware_toolchain_guide.md)

## Directory Structure
```
.
├── build                               - Directory for compilation output.
├── common                              - STM32 common library, see the readme in here for more details.
├── config                              - ChibiOS configuration files.
│   ├── board.chcfg                     - Defines the pin mapping and clock frequency of the board.
│   ├── chconf.h                        - ChibiOS RT configuration.
│   ├── halconf.h                       - ChibiOS HAL configuration.
│   ├── mcuconf.h                       - ChibiOS HAL driver configuration.
├── doc                                 - Documentation folder.
│   ├── chibios                         - ChibiOS documentation.
│   ├── datasheets                      - Datasheets of important components on this board.
│   ├── schematics                      - Schematics of this and related boards.
│   └── software                        - Software documentation.
├── makefile                            - Makefile for this application.
└── src                                 - C source / include files.
    ├── can                             - Code related to this device's CAN interface. This defines the messages this board
    │                                     transmits and receives.
    ├── controls                        - Code related to control systems. Torque vectoring implementations are defined here.
    └── peripherals                     - Code related to board hardware and peripherals.
```