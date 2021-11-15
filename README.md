# 8-Up-Battery-Charger

For a battery fixture at work, but nothing too fancy. Schematic or firmware could be useful for:  
Atmega32u4 -- ADC reading on multiple channels, installing USB bootloader and using as Arduino  
TI BQ24079 and similar ICs reference design

Specs:   
To be able to charge 8 batteries at at time for production and soldering/ welding -- they have to be charged, but not to max charge.  
Charger IC shall not have default max charge above 4.1V.   
The batteries are in metal cans, with pogo pins that connect to each terminal -- board layout must match mechanical design of fixture.   
Batteries shall be able to be installed without reference to polarity.  
Green LEDs shall indicate charging, blue LEDs shall indicate finished charge.  
Pushbutton reset.

An H-bridge circuit was used to ignore battery polarity, so that the operator would not have to worry about that when installing the batteries.

The TI BQ24079 battery charger IC was used, so that the maximum charge voltage is 4.1V, in case there was a firmware issue and charging was not disabled. The max voltage on our battery was stated in the datasheet as 4.1V. There are other ICs in the same TI family with same or similar pinouts with 4.2V max charging, if that is required. Charging is set at 30mA, for a 60mAh battery at 0.5C charge rate. Green LEDs indicate the circuit is charging, which is driven by the charger IC. 

The micro, an Atmega32u4, was used to turn off charging for any battery that hit a nominal voltage such as 4.0V. As such, it uses 8 ADC inputs directly from the battery's H-bridge. It uses 8 outputs to disable battery charger ICs as needed with a 5V digital output, which also turns on a blue LED to indicate charging is completed. A Reset pushbutton is used to re-initialize for new batteries.

The board was laid out in Altium. Since it was for work, that file is not shared -- only a PDF schematic. Anyway, it would not be useful without the fixture and our specific, VERY expensive batteries.   
Included is the correct footprint for the power connector, the PJ-202A. Thankfully I checked the footprint I found online first. Not only did it use big circles instead of the appropriate slots for the pins, but it was completely backwards, and the jack would have been on the bottom of the board!

Firmware was done in Atmel/ Microchip Studio and flashed via ISP, using a Tag Connect cable. A USB connector was included in the circuit design in case we wanted to do firmware updated with USB. In this case, I would have used the Arduino IDE and changed the code to work with the Arduino setup/ loop paradigm. I've included a document on how to put the Leonardo USB bootloader on the Atmega32u4 with ISP, and how to package the bootloader, main code and fuses for a production .elf file.   
I have yet to be successful using other USB bootloaders for the 32u4 other than Arduino. If you've had success with that, please share!  
Since the firmware is so compact and simple, I kept it all to one file. 

This was my first project as this job where they let me do the design, board layout, assembly (stencil, heat gun and an iron) and firmware. Those are my favorite projects, especially when they work. 
