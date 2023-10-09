--------------------------------------------------------------------------------
Clock with calendar from Gotek's LED screen parts
Copyright (c) 2022-2023 Wierzbowsky [RBSC]
--------------------------------------------------------------------------------

About
-----

This is the the self-made clock that was created from salvaged Gotek's 7-segment LED screen boards. The original Gotek
screen boards need to be dismantled and all parts (IC, screen, capacitor) are used to create a single or combined
indicator panel with the help of tiny circuit boards that can be joined together. These boards share all signals except
for DIO. Each DIO pin is connected to the corresponding pin of the Arduino Mini, Nano or Uno board. This particular clock
has 6 screens joined together.

The screens can display decimal and hexadecimal numbers with or without prepending zeroes, single digits and characters
at defined positions, all Latin alphabet and also several special characters (minus, plus, slash, etc). The original
Arduino library for the TM1651 chip created by Derek Cooper was almost completely rewritten and enriched with features.
A few subroutines from the library were imported into the .INO file to make things easier.

The Arduino board with the custom self-made firmware allows to set the time and date, select 3 brightness levels and also
to enable the auto-brightness mode. A user can enable the hourly beep during day hours and set the adjustment to slow
down or make the clock run faster. The total consumption of the clock with Arduino board is around 100mA, so powering it
up from a USB2 port is fine.


Assembling
----------

Take 6 orginal Gotek LED screen boards and desolder the LED screen, the controller chip and the capacitor. The solder the
chip, capacitor and the screen onto the small board (you can order it from jlcpcb.com factory using the provided Gerber
files).

Solder a 4-pin single row header to the right side of one assembled board. To make it easier, it's recommended to solder
the 4-pin header BEFORE soldering the screen. Cut the popping out pins of the pin header from the other side so that the
screen could be fully set onto the board.

Assemble the 2-row 3-column display from the 6 complete boards. Make sure that there's no gap between the screens. To join
the boards you can use U shaped pins cut from the pin headers. See the picture of the assembled screen for reference.

Connect power, ground and clock lines of each screens in a row together. Solder 3 wires between corresponding solder pads
of power, ground and clock signals of the upper row and the bottom row. See the picture of the assembled screen.

Finally, solder four 2-pin headers onto the DIO solder pads of each board. 


Connections
-----------

Connect each board's DIO pin to the digitap I/O pin of the Arduino Uno or Nano board. Please note thet the board has DIO
pads on each side, so please connect only ONE of them (one DIO signal for each screen). Connecting the DIO pins should be
done in the specific order so that the pin the the smallest number is connected to the leftmost screen.

In my setup the DIO pins are connected to the Arduino board the following way:

-------------------------
|---3---|---4---|---5---|
-------------------------
|---6---|---7---|---8---|
-------------------------

Connect the 3.3v power from the Arduino board to the 4-pin header's VCC pin. Connect the ground from the Arduino board to
the 4-pin header's GND pin. Connect the I/O pin 3 from the Arduino board to the 4-pin header's CLK pin (this is clock
signal). You can change the pin assignment in the source files if you wish. The current pin assignment is as follows:

Arduino		Clock board or beeper/button
--------------------------------------------
3.3v		VCC
GND		GND
 2		CLK
 3		DIO pin of screen 1
 4		DIO pin of screen 2
 5		DIO pin of screen 3
 6		DIO pin of screen 4
 7		DIO pin of screen 5
 8		DIO pin of screen 6
12		Beeper (optional)
13		Button
GND		Beeper's ground
GND             Button's ground

The piezo beeper is optional, but recommended. You can use a simple 2-pin beeper in a barrel-shaped case. It can be found
anywhere on AliExpress or Ebay. For example: https://www.ebay.com/itm/374210416844


Operation
---------

The clock is controlled by a single pushbutton. There are 3 types of commands that you can execute. A short pushdown (less
than 1 second push), a medium pushdown (over 1 second push) and a long pushdown (over 2 second push). It's all about time.
Each operation is acknowledged by the clock with a beep of different frequency and duration.

When the clock shows date and time, a short pushdown will change the brightness mode and disable the auto-brightness mode if
if was previously set. There are 3 different brightness levels to select.

The medium pushdown will allow to enter the settings mode (see below). The long pushdown switches on the auto-brightness
mode. The brightness will automatically change based on time of the day. From 8:00 to 20:00 normal brightness is set,
otherwise low brightness is set.

In the settings mode a short pushdown will change the value of hour, minute, day, month, year, select beep ON or beep OFF
and allow to set the adjustment in the range of -15 to 15 (slow/fast) for the clock. A short pushdown will zero the value of
seconds if seconds are blinking.

The medium pushdown will allow to select the setting that needs to be changed (hour, minute, day, etc.). The currently
selected setting will blink. The selection is done in a loop, for example: second->minute->hour->second...

The long pushdown will switch from time setting mode to date setting mode, then to beep setting mode and finally to the
clock adjustment setting mode. After the last mode the newly-set date/time will be shown and the clock will start running.

It may be tricky to change settings for the first time, but you will get used to it pretty fast.


Operation and test videos
-------------------------

You can see the videos of the clock's testing on Youtube:

 - https://www.youtube.com/watch?v=1-ujJE8o4cY
 - https://www.youtube.com/watch?v=EFognT4_trQ


IMPORTANT!
----------

I provide all files and information for free, without any liability (see the disclaimer.txt file). The provided information,
software or hardware must not be used for commercial purposes unless permitted by the owner. Producing a small amount of
bare boards for personal projects and selling the rest of the batch is allowed without the permission.

When the sources files are used to create alternative projects, please always mention the original source and the copyright!


Contact information
-------------------

The members of RBSC group Tnt23, Wierzbowsky, Pyhesty, Ptero, GreyWolf, SuperMax, VWarlock and DJS3000 can be contacted via the group's e-mail
address:

info@rbsc.su

The group's coordinator could be reached via this e-mail address:

admin@rbsc.su

The group's website can be found here:

https://rbsc.su/
https://rbsc.su/ru

The RBSC's hardware repository can be found here:

https://github.com/rbsc

The RBSC's 3D model repository can be found here:

https://www.thingiverse.com/groups/rbsc/things

-= ! MSX FOREVER ! =-

