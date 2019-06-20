# arduino-i2c-rw-eeprom

Dump an I2C EEPROM to your PC via an Arduino

Added support for writing from file to memory, check options -w and -i <input_file>

Examples:

read 24C16 EEPROM and print on terminal

`./i2c_rw_eeprom -d 1 -t /dev/ttyUSB0 -n 2048 ; hexdump -C eeprom.bin`

write 24C16 EEPROM

`./i2c_rw_eeprom -d 1 -t /dev/ttyUSB0 -n 2048 -w -i eeprom.bin`

read 24C02 EEPROM and print on terminal

`./i2c_rw_eeprom -d 1 -t /dev/ttyUSB0 -n 256 ; hexdump -C eeprom.bin`

write 24C02 EEPROM

`./i2c_rw_eeprom -d 1 -t /dev/ttyUSB0 -n 256 -w -i eeprom.bin`


Table of contents
-----------------

- [Original work](#original-work)
- [Introduction](#introduction)
- [How to](#how-to)
- [New features](#new-features)
- [EEPROMs type](#eeproms-type)
- [Compatibility](#compatibility)
	- [TTY device names](#tty-device-names)
	- [Tested EEPROMs](#tested-eeproms)
- [Example](#example)
	- [Setup](#setup)
	- [Reading EEPROMs](#reading-eeproms)
- [TODO](#todo)
- [License](#license)



Original work
------------

This project is based on the original work by: [andre-richter/arduino-spi-read-eeprom](https://github.com/andre-richter/arduino-spi-read-eeprom)



Introduction
------------

While I was searching for a way to dump an entire I2C EEPROM and save it as a binary file on the PC, I stumbled on the original work, mentioned above, for SPI EEPROMs.

As it uses a small program written in C, I decided to use it as a base for my work with I2C devices.



How to
------------
The simple configuration to read an I2C EEPROM is:

1. Connect the EEPROM to your Arduino's like [this](http://kamilslab.com/wp-content/uploads/2015/12/eeprom.jpg) or [this](http://fritzing.org/media/fritzing-repo/projects/r/readwrite-serial-eeprom-via-i2c/images/arduino-eeprom_bb.png)
    * Be sure about the AX pins on some EEPROMs
2. Connect the Arduino to your PC via the USB __programming port__ and upload the sketch.
3. Compile the C program with a simple `make`
4. Run the program and supply the __MINIMAL__ arguments:
    * The tty device name of your Arduino e.g. `-t /dev/ttyACM0`
    * The number of bytes you want to read from the EEPROM: `-n 8`

The read bytes will be stored in a file called __eeprom.bin__



New features
------------

On the Arduino sketch:
- Is the same simple sketch in which the SPI interface was switched for the I2C interface (Wire library).
	- Some tutorials on using the Wire library, such as the [Wire library](https://www.pjrc.com/teensy/td_libs_Wire.html) and also other works by [breadboardtronics/EEPROM and arduino](https://breadboardtronics.wordpress.com/2013/08/27/at24c32-eeprom-and-arduino).
	- Também existem tutorias em PT, [exemplo](http://lusorobotica.com/index.php/topic,33.0.html) =) 
- The number of bytes passed for input was increased from 4 to 5, allowing to read more devices.

On the C program there are new features such as:
- It can save the output to a given file name (the user gives the name it wants)
- While reading the memory it's content may not be printed to the terminal (since it was annoying printing the whole memory therefore fill in the terminal)
- It can read from input n Kbytes with the program converting Kbytes to bytes, e.g. input 50k => 51200 bytes
- The type of addressing of the device can be specified with `-d` or `--dev-type` allowing to read multiple devices
- The overall program was enhanced, by preventing some errors in the user's input
- ~~Still work's with SPI EEPROMs~~



EEPROMs type
----------------------
Since there are multiple ways to read the whole content of EEPROMs bigger than 64Kbytes(512Kbit), a special command must be set.

Also, to read smaller EEPROMs with 16bytes(128bits) to 2Kbytes(16Kbits), a special command must be set.

### Default
EEPROMs bigger than 4Kbytes(32 Kbits), example: 24XX32 ... 24XX1026, 24X02, ...
![default](http://s16.postimg.org/3vk5ee46d/Default.png "Default")


### Smaller
EEPROMs up to 2Kbytes(16Kbits), example: 24XX00 ... 24XX16, ...

**Must set flag** `-d 1` or `--dev-type=1`
![type_1](http://s18.postimg.org/gud8exihl/EEPROM1.png "type 1")


### Other type 1
Example compatible: 24XX1025, ...

**Must set flag** `-d 2` or `--dev-type=2`
![type_2](http://s23.postimg.org/59onbo1wb/EEPROM2.png "type 2")



Compatibility
----------------------
I have tested the program with an Arduino UNO on Debian@3.16.0-4-amd64 with gcc 4.9.2, but it should work with every POSIX compatible OS.


### TTY device names
- In Mac should be the same as the original `/dev/cu.usbmodem14931` (BUT not tested by me)
- In Linux is `/dev/ttyACM*`, where * should be 0. (you need to be root, or be in [group-mode](http://playground.arduino.cc/Linux/All#Permission))


### Tested EEPROMs
I've tested with the following devices:
 - AT24C16
 - AT24C32
 - GT24C64
 - 24FC1025
 - Other's I2C EEPROMs compatible with the read protocol (or feel free to do the necessary changes)



Example
------------


### Setup
```
$ git clone git@github.com:DMRodrigues/arduino-i2c-read-eeprom.git
$ cd arduino-i2c-read-eeprom
$ make
```


### Reading EEPROMs
To read 1024Kbits from an 24XX1026 and save it without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128k -p n`

To read 16Kbits from an 24XX16 and save it without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 2k -p n -d 1`

To read 1024Kbits from an 24XX1025 and save it without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128k -p n -d 2`

To read 1024bits and save it under the name test.bin and printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128 -o test.bin`

To read 64Kbytes and save it under the name large.bin without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 64k -o dump.bin -p n`

To read 10Kbytes as text(ascii) and save it under the name readme.txt without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 10k -o large.txt -f a -p n`

Afterwards, you can edit the data with your favourite hex editor.



TODO
------------
- Improve C program regarding interrupt signal and clean exit function in case of errors

- Read from a start address to a end address



License
------------

The MIT License (MIT)

Copyright (c) 2016 Diogo Miguel Rodrigues

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

