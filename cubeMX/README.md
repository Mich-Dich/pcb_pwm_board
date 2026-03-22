
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
```
rm -rf build
cmake -S . -B build --preset Debug
cmake --build build
```
OR
```
rm -rf build && cmake -S . -B build --preset Debug && cmake --build build
```



sudo apt update
sudo apt remove openocd gdb-multiarch



## VSCode Extensions
marus25.cortex-debug
ms-vscode.cmake-tools
