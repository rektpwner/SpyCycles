# SpyCycles

## Install

```
sudo apt-get install libcapstone-dev
```

## Compile

```
gcc -W -Wall spycycles.c -lcapstone -o spycycles
```

Or using cmake:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug # Or Release
cmake --build build
cmake --install build # To install the binary (might need sudo)
```

## Run

```
./spycycles ./hello_world
```
