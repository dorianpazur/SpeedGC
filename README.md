# 🎮 Games Fleadh 2026  

## SETU Carlow: Department of Computing

# 🕹️ Game Title  
**SpeedGC**

## 👥 Team Members  
- Dorian Pazur : Engine Programmer, Art
- Zaid Elkhazendar : Gameplay Programmer

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
5. Run build:
   ### If in Dolphin:
   Open ntsc_u.iso
   ### If on hardware:
   Copy the Speed folder in Build to the root of your SD card and run Speed.dol using Swiss

### Option 3 – Run on Hardware
1. Download unpacked build
2. Extract Speed directory to root of SD card
3. Run Speed.dol in Swiss
