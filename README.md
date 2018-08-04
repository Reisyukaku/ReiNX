# ReiNX
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

*The modular switch custom firmware*

**Usage:**

Put `ReiNX` folder on the root of your switch's SD card and run `ReiNX.bin` with your favorite fusee launcher.

**Compiling:**

You'll need devkitpro with devkitARM and run `make`

To compile with Docker, `chmod +x docker-build.sh` and run the shell script `./docker-build.sh`. This will compile without requiring installation of DevKit* dependencies.


**Features:**

* Loads all KIPs from `/ReiNX/sysmodules/` directory

* Optional custom kernel/secmon/warmboot

* FS patches on the fly (NCA verify/cmac and optional nogc)

* Exclusive ReiNX sysmodules with built in ES patches

**Official thread:**

[HERE](https://gbatemp.net/threads/official-reinx-thread.512203/)

**Credits:**
 
 Naehrwert for hardware init code and generally being helpful!
 
 CTCaer and st4rk for their contribution to the hardware code aswell!
 
 SciresM for sysmodules!
 
 The community for your support!
