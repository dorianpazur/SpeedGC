# 🎮 Games Fleadh 2026  

## SETU Carlow: Department of Computing

# 🕹️ Game Title  
**SpeedGC**

## 👥 Team Members  
- Dorian Pazur : Engine Programmer, Art
- Zaid Elkhazendar : Gameplay Programmer
- Callum Nolan : Audio

## 📝 Brief Description  
SpeedGC is an arcade racer whose objective is to collect batteries to keep your energy high and outrun your buddy to the finish line.
There are concrete barriers littered throughout the motorway that you need to dodge otherwise you'll stop dead in your tracks and have to use precious energy to get up and running again,
and your buddy might just get ahead and take all your batteries!

Your energy meter helps you keep track of how much energy you have, if it runs out, your car will still drive, sure, but it'll slow down a lot, so keep collecting those batteries!

## 🎮 Gameplay Overview  

- **Genre:** Racing, Arcade  
- **Platform:** Nintendo GameCube
- **Game Engine:** Custom
- **Controls:** L to brake/reverse or reset when flipped, R to accelerate, Left stick to steer
- **Number of Players:** 1-2

## 🔗 Project Links  

- 💻 **Source Code:**  
  [SpeedGC](https://github.com/dorianpazur/SpeedGC)
  
- 💻 **Executable Code:**  
  [SpeedGC](https://github.com/dorianpazur/SpeedGC)
  
- 🎬 **Trailer Video:**  
  [Trailer](https://github.com/dorianpazur/SpeedGC/blob/main/Trailer.mp4)

## 🛠️ Installation & Running the Game  

### Option 1 – Run Build in Dolphin
1. Download the latest ISO release from the Releases section.
2. Extract the zip file.
4. Run the ISO in Dolphin Emulator.

### Option 2 – Run from Source
1. [Install devkitPro](https://devkitpro.org/wiki/Getting_Started)
2. Clone the repository:
  (NB: destination path must not include spaces!)
   ```bash
   git clone https://github.com/dorianpazur/SpeedGC.git
   ```
4. Build the project:
   ```bash
   make -j8
   ```
   Note: The first build will fail as it needs to generate a revision file. Run make again and it should build then.

5. Run build:
   ### If in Dolphin:
   Open ntsc_u.iso
   ### If on hardware:
   Copy the Speed folder in Build to the root of your SD card and run Speed.dol using Swiss

### Option 3 – Run on Hardware
1. Download unpacked build
2. Extract Speed directory to root of SD card
3. Run Speed.dol in Swiss

# License
BSD 3-Clause License

Copyright (c) 2019, Electronic Arts
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
