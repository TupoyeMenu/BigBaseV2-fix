# BigBaseV2
A mod menu base for Grand Theft Auto V.
Strictly for educational purposes.

## Features
* ImGui–based user interface
* Unloading at runtime
* Log console
* Script fibers
* Fiber pool
* Access to YSC script globals
* scrProgram–based native hook
* MinGW compilation support


## Building

### Windows
To build BigBaseV2 you need:

* Visual Studio 2019 or higher
* [CMake](https://cmake.org/download)

To set up the build environment, run the following commands in a terminal:
```bash
git clone https://github.com/TupoyeMenu/BigBaseV2-fix.git
cd BigBaseV2-fix
mkdir build
cd build
cmake ..
```
Now, you will be able to open the solution, and simply build it in Visual Studio.

### GNU/Linux
To build BigBaseV2 you need:

* MinGW 12 or higher
* CMake
* Ninja? Didn't test make.

To set up the build environment, run the following commands in a terminal:
```bash
git clone https://github.com/TupoyeMenu/BigBaseV2-fix.git
cd BigBaseV2-fix
mkdir build && cd build
cmake .. -DCROSSCOMPILE:BOOL=TRUE -DUSE_GCC:BOOL=TRUE -G Ninja
ninja
```

## Credits
| Credits                                                    |                                                                        |
| ---------------------------------------------------------- | ---------------------------------------------------------------------- |
| Pocakking                                                  | [BigBaseV2](https://github.com/Pocakking/BigBaseV2)                    |
| spankerincrease(gir489)                                    | [BigBaseV2-fix](https://bitbucket.org/gir489/bigbasev2-fix)            |
| [YimMenu](https://github.com/YimMenu/YimMenu) contributors | Updates for newer GTA V versions, most of the code is stolen from them |
| tupoy-ya                                                   | Being stupid                                                           |

## License
This project is licensed under [GNU GENERAL PUBLIC LICENSE Version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt) because i stole a lot of code from YimMenu.