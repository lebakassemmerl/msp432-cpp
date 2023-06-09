# MSP432-C++

This project is completely written in C++ 20. The standard lib is not used, everything is written
from scratch. This is more like a playground or proof of concept to show and test how C++ can be
used for bare metal micro-controller programming. There may be several bugs in this project, just
open an issue or pull-request if you have time :)

The API/Design is heavily inspired by the API of Tock (https://github.com/tock/tock).

## What works
This project is nowhere near feature complete. It is still heavily under development. But a few
things are still working already:
- GPIO output and input (without Interrupt functionality)
- DMA
- UART
- SPI (theoretically but it's not tested yet)

## What has to be done
- [ ] A proper way to describe the defined bitfield-values of the peripheral registers
- [x] FPU support
- [ ] Missing peripheral drivers
- [ ] Rewrite of buildsystem (for some reason `make rebuild -j` does not work)
- [x] Rewrite of `Fifo` (especially an `emplace()` function via variadic templates)
- [/] Integrate Testcases into build-system

## How to use
Every project has to be located in the `projects` directory. Simply create a new folder and copy the
makefile from `projects/test/` and adapt it to your needs. Compile the project with `make`, clean
the project with `make clean`, rebuild the project with `make rebuild` and flash the binary to the
target with `make flash`. I was developing this code with VSCode on Archlinux, thus I programmed the
target via openOCD (`openocd.cfg`).

## Hardware
This codebase is designed for the MSP432P401R microcontroller, which consists of a Cortex M4F and
the "old" peripherals of the famous MSP430 family from Texas instrumens. The reason why I chose this
board was that I had one lying around from my masters thesis and the peripherals are very easy and
simply to use. Thus it was/is easier to focus on the "C++-Side". I think the MSP-EXP432P401R
evaluation board is not available anymore, but the MSP432E4xx should be compatible except the DMA.

- Evaluation Board: https://www.ti.com/tool/MSP-EXP432P401R
- Device Specific Datasheet: https://www.ti.com/lit/ds/slas826e/slas826e.pdf
- User Guide: http://edge.rit.edu/edge/P17363/public/MSD2%20Documents/msp432%20technical%20reference.pdf
