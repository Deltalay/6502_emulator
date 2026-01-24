# 6502 Emulator

6502 Emulator is a project to emulate the classic 6502 microprocessor.  
This project is **currently under development**, so features, APIs, and functionality may change frequently.

> **Disclaimer:** This emulator does **not yet support cycle-accurate emulation**. Timing may be approximate and some programs may not behave correctly.

## Features

- Emulates the 6502 CPU instruction set
- Basic memory and register support
- Build system ready with CMake

## Requirements

- C compiler (tested with GCC/Clang)
- CMake 3.10 or higher
- Linux or macOS (Windows support may require tweaks)

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## License

This project is licensed under the [GNU General Public License v3.0 (GPL-3.0)](LICENSE).  

> This program is free software: you can redistribute it and/or modify  
> it under the terms of the GNU General Public License as published by  
> the Free Software Foundation, either version 3 of the License, or  
> (at your option) any later version.  
>
> This program is distributed in the hope that it will be useful,  
> but WITHOUT ANY WARRANTY; without even the implied warranty of  
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  
> GNU General Public License for more details.
