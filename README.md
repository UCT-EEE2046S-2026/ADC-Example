# EEE2046S_STM32F0_Template_Project

UCT EEE2046S Template Project for the UCT Developemnt Board.

## Overview

This is a general purpose starter repository intended for programming the UCT Development Board with minimal stoftware bloat.

## Getting Started

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/UCT-EEE2046S-2026/EEE2046S_STM32F0_Template_Project YOUR_PROJECT_FOLDER_NAME
   ```

>[!NOTE]
> You may require `git` to be installed on your machine.
> Run `git --version` to check if it is installed. If not, install it with:
>
> - Windows: `winget install Git.Git`
> - MacOS: `brew install git`
>
> Alternatively, you can download the repository as a ZIP file and extract it to your desired location.
  
2. Install all system dependencies (things your computer needs installed to build the project). Two automated scripts are provided to install most of the dependencies for Windows and MacOS:

   - Windows: [InstallSystemDependencies-Windows.bat](InstallSystemDependencies-Windows.bat)
  Simply double-click the script in Windows Explorer or run it from the command line like so:

     ```cmd
     .\InstallSystemDependencies-Windows.bat
     ```

   - MacOS: [InstallSystemDependencies-MacOS.bash](InstallSystemDependencies-MacOS.bash)
    Run the script from the terminal like so:

     ```bash
        bash InstallSystemDependencies-MacOS.bash
     ```

> [!TIP]
> You can also open these files and copy-paste each line into your terminal to install the dependencies manually.

> [!NOTE]
> You may need to restart your terminal and/or VS Code after installing dependencies before the newly installed commands are available.

3. Open the project in Visual Studio Code (VS Code). You can do this by opening VS Code and selecting `File > Open Folder...` and navigating to the folder where you cloned the repository.

4. Open a terminal in VS Code by selecting `Terminal > New Terminal` from the menu or by using the keyboard shortcut.

5. Configure the CMake Preset for the project:

   ```bash
   cmake --preset Debug
   ```

6. Build the project with CMake:

   ```bash
   cmake --build --preset Debug
   ```

   >[!TIP]
   > You can also use the VS Code command palette (Ctrl+Shift+P or Cmd+Shift+P) and search for "CMake: Build" to build the project.

7. Connect the UCT Development Board to your computer via USB. Make sure there are no issues with the USB connection.

8. Flash the built binary to the UCT Development Board using the "Debug with OpenOCD" launch configuration in VS Code. You can do this by opening the command palette (Ctrl+Shift+P or Cmd+Shift+P) and searching for "Debug: Start Debugging" or by pressing F5.

If successful, you should see the playback debugging controls appear in the top bar of VS Code, and the program should halt at the `main()` function in `Src/main.c`. You can now press Run (▶) to continue execution and see the program running on the UCT Development Board.


## Repository Structure

This repository contains a non trivial file structure and build system. This section will outline the folder structure and build process.

### [Inc](Inc) & [Src](Src)

Main application Include and Source directories.

Source (`.c`) files should be placed in the `Src` folder, and header (`.h`) files should be placed in the `Inc` folder.
All source files are automatically included in the build process via the CMake build system.
If additional include directories are required, they can be added within the main [`CMakeLists.txt`](CMakeLists.txt) file.

You should add new source files to the `Src` folder and new header files to the `Inc` folder as needed to seperate your code into logical modules.

The following source files are required boilerplate for the project to build successfully:

| File | Description |
| ---- | ----------- |
| [main.c](Src/main.c) | Main application entry point. This file contains the `main()` function, which is the starting point of the application. |
| [stm32f0xx_it.c](Src/stm32f0xx_it.c) | Interrupt Service Routines (ISRs) for the STM32F0 target. This file contains the interrupt handlers for various peripherals and system exceptions, some of which are required for the project to build successfully. |
| [syscalls.c](Src/syscalls.c) | System call implementations. These are basic functions that the C standard library normally expects to be provided by the operating system. In this case, they are implemented as "stubs" to prevent linker errors, but they do not provide any actual functionality. |
| [sysmem.c](Src/sysmem.c) | Memory management functions. These functions are used by the C standard library for dynamic memory allocation. **Note:** No dynamic memory allocation is used in this project. Using `malloc()` will succeed by calling `_sbrk()` in `sysmem.c`, but the memory can never be freed, and the heap will eventually collide with the stack, crashing the program. **Never use `malloc()` in this project.** |

### [Drivers](Drivers)

This folder contains CMSIS and STM32F0 Header files. These files are provided by ST and are not intended to be modified.

These files are linked to the main executable via the CMake build system and provide header files for STM32F0 peripherals register definitions and structures. The generic `stm32f0xx.h` header file provides access to all STM32F0 peripheral register definitions and structures.

Example:

```c
#include "stm32f0xx.h"

void main(void) {
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA5 as output
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    while (1) {
        // Toggle PA5
        GPIOA->ODR ^= GPIO_ODR_5;
        for (volatile int i = 0; i < 100000; i++); // Delay
    }
}
```

### [CMake](CMake)

This folder contains key CMake files for the build system. These files are not intended to be modified.

| File | Description |
| ---- | ----------- |
| [arm-none-eabi.cmake](CMake/arm-none-eabi.cmake) | Specifies CMake to use the ARM GCC Embedded toolchain file used for cross-compiling to the STM32F0 target. |
| [mcu.cmake](CMake/mcu.cmake) | Specifies the STM32F0 target MCU for the build system. |
| [scripts](CMake/scripts) | Identifies the Linker and Startup files needed for the cross-compile build. |

### [.vscode](.vscode)

This folder contains Visual Studio Code specific configuration files.

| File | Description |
| ---- | ----------- |
| [extensions.json](.vscode/extensions.json) | Recommended VS Code extensions for this project. |
| [settings.json](.vscode/settings.json) | VS Code settings for this project, specifically Clang formatting. |
| [launch.json](.vscode/launch.json) | Debugging configurations for VS Code, including OpenOCD Debug configuration for the STM32F0 target. |

### [Root Files](.)

Files in the top-level directory of the repository.

| File | Description |
| ---- | ----------- |
| [CMakeLists.txt](CMakeLists.txt) | Main CMake file for the project. This orchestrates the cross-compile build process, specifying the source files, include directories, compiler and linker flags, and the output binary format. Edit `project(MODIFY_PROJECT_NAME)` to change the project name. |
| [CMakePresets.json](CMakePresets.json) | CMake Presets file for the project. This file defines build configurations, including the toolchain (ARM GCC Embedded), build type (Debug/Release), and build output directories (Build/). |
| [startup_stm32f051r8tx.s](startup_stm32f051r8tx.s) | Startup file for the STM32F0 target. This file is responsible for setting up the initial stack pointer, initializing the .data and .bss sections, and calling the main function. |
| [STM32F051R8Tx_FLASH.ld](STM32F051R8Tx_FLASH.ld) | Linker script for the STM32F0 target. This file defines the memory layout of the microcontroller, including the flash and RAM regions, and specifies how the sections of the program are placed in memory. |
| [openocd.cfg](openocd.cfg) | OpenOCD configuration file for the STM32F0 target. This file specifies the interface and target settings for OpenOCD to communicate between the host computer and the STM32F0 microcontroller. |
| [.gitignore](.gitignore) | Git ignore file for the project. This file specifies which files and directories should be ignored by Git, such as build artefacts and temporary files. |
| [.clang-format](.clang-format) | Clang formatting configuration file for the project. This file defines the coding style and formatting rules for C files in the project. |
| [README.md](README.md) | This file. |

