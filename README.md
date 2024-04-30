# Amiga-PS2-Mouse-using-Arduino for a touchpad internaly connected
Connect a PS/2 Mouse or PS/2 Touchpad internaly to an Amiga.
It does supports up to 3 buttons mices, acceleration and tap click on touchpad.
This version includes a timer and disable itself after 5 secondes of inactivity. This allow the use of mouse or joystik plugged to the DB9 externaly while this version is soldered in the DB9 pins internaly.
It resumes automatically when the touchpad or mouse plusgged internaly moves or click.

## What is needed:

  - An Arduino (Nano/Uno/Mini/...)
  - A PS/2 Mouse ou Touchpad (or a USB mouse with a USB to PS/2 adapter)
  - Female DB9 (to connect to the Amiga port externaly)
  
## Connection:
```
  PS/2              Arduino
  ----              -------
  CLOCK              D11 
  DATA               D12
  5V                 5V
  GND                GND
  
  DB9               Arduino
  ---               -------
  1                  D2
  2                  D3
  3                  D4
  4                  D5
  5                  D8
  6                  D6
  7                  VIN
  8                  GND 
  9                  D7
```

## Software

Get the PS2Mouse library from Jacek Kunicki: https://github.com/rucek/arduino-ps2-mouse
Compile the INO project and upload it to the Arduino.

## Usage

Plug the DB9 on the mouse port of the Amiga, mouse should work as an Amiga mouse.

## Thanks to:

  - Jacek Kunicki (rucek) for the PS2Mouse library:
    https://github.com/rucek/arduino-ps2-mouse
  
  
  - Spark (cssvb94) for AmigaUSBMouseJoystick:
    https://github.com/cssvb94/AmigaUSBMouseJoystick


  - Don M (glitterkitty) for the old version basic version of this code not working anymore
    https://github.com/glitterkitty/Arduino-PS2-Mouse-to-Amiga-Adapter
  


 
  


