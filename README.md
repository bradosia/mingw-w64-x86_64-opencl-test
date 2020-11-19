# mingw-w64-x86_64-opencl-test

ALL credit to https://github.com/Dakkers/OpenCL-examples

Only addition is cmake files for mingw-w64

# Build Instructions

## Windows - MinGW-w64
Install MSYS2<BR>
Then, install GCC, cmake, git and dependencies. Confirmed working with Windows 10 as of 11/18/2020.
```shell
pacman -Syu
pacman -S mingw-w64-x86_64-gcc cmake git
pacman -S mingw-w64-x86_64-opencl-headers
```
Build:
```shell
git clone https://github.com/bradosia/mingw-w64-x86_64-opencl-test
cd mingw-w64-x86_64-opencl-test
mkdir build
cd build
cmake -G "MSYS Makefiles" ../
make
```
