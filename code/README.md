
# STUFF 

rm -rf build
cmake -S . -B build
cmake --build build

rm -rf build && cmake -S . -B build && cmake --build build

## install required packages
```
sudo apt update
sudo apt install gcc-arm-none-eabi
```

## Build

Create the elf file
```
rm -rf build && cmake -S . -B build --preset Debug && cmake --build build
```

sudo apt update
sudo apt remove openocd gdb-multiarch

## VSCode Extensions
marus25.cortex-debug
ms-vscode.cmake-tools


# CLI commands

Erase all
```
STM32_Programmer_CLI --connect port=SWD --erase all
```
