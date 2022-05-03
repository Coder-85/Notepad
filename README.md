# Notepad

A simple text editor

## Build From Source
1. Clone this repository or download zip.
```
$ git clone https://github.com/Coder-85/Notepad
```
2. To configure, run the following command in the root directory.
```
$ cmake -DCMAKE_PREFIX_PATH="path/to/Qt6/lib/cmake" -G "<generator_name>" 
-S . -B build
```
3. To build, run the following command in the root directory.
```
$ cmake --build build --target all
```

## Prebuilt Binaries
Prebuilt binaries for windows(x86) can be found in the releases page.