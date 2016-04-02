# arduino-i2c-read-eeprom

Dump an I2C EEPROM to your PC via an Arduino


Table of contents
-----------------

- [Original work](#original-work)
- [Introduction](#introduction)
- [How to](#how-to)
- [New features](#new-features)
- [Devices address type](#devices-address-type)
- [Compatibility](#compatibility)
	- [TTY device names](#tty-device-names)
	- [Compatible EEPROMs](#compatible-eeproms)
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
    * Be sure about the A2 pin some EEPROMs must be hard wired to Vcc (e.g. 24FC1025)
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
- The number of bytes passed for input was increased from 4 to 6, allowing to read up to 2Mbits

On the C program there are new features such as:
- It can save the output to a given file name (the user gives the name it wants)
- While reading the memory it's content may not be printed to the terminal (since it was annoying printing the whole memory therefore fill in the terminal)
- It can read from input n Kbytes with the program converting Kbytes to bytes, e.g. input 50k => 51200 bytes
- The type of addressing of the device can be specified with `-d` or `--dev-type` allowing to read more than 64Kbytes
- The overall program was enhanced, by preventing some errors in the user's input
- ~~Still work's with SPI EEPROMs~~



Devices address type
----------------------
Since there are multiple ways to read the whole content of EEPROM bigger than 64Kbytes or 512Kbit, a special command must be set.

### EEPROM type 1
Must set flag `-d 1` or `--dev-type=1`
Some examples are: 24XX1026, M24M01, CAT24M01, AT24CM01, ...
![type_1](https://ch3302files.storage.live.com/y3mxZhsnuO_9DcPUrTW97842UpC7beExyoSk3QyEiGdFp9on653JUQhX0QoWSAIo19gzfJeNC1Edyqk5dGZJx2DHy5K0nxpYT4s0PxcIIIK5smDBg-nZVA6gDzKD65T4x18ZSLVVDvAN-CdaRsIX0jMaEfk7MJNn6LHItQ4uzhEiTU/EEPROM1.png?psid=1&width=412&height=212 "EEPROM type 1")


### EEPROM type 2
Must set flag `-d 2` or `--dev-type=2`
Some examples are: 24XX1025 ...
![type_1](https://ch3302files.storage.live.com/y3mgOuztKNg2NeLvU86fG5qpaJqoo2f_xQ480PDv6Es6-Dxom6_HsFbfwnRAnFu59JinNi6fsCEnvU76W7uzpGoU-EIvgO3ZI1iJTkZpKbnjFe3C3E2GqODWwr1EwRV4FWYdQ_NHIfdFpaMjg7frnRqjgDQserS-ABrPoR0S7aomPM/EEPROM2.png?psid=1&width=412&height=212 "EEPROM type 2")


### EEPROM type 3
Must set flag `-d 3` or `--dev-type=3`
Some examples are: AT24CM02, M24M02, ...
![type_1](https://ch3302files.storage.live.com/y3m9FKJ-_urTKHPavZ8iY2AVayBbwfM4O2oof9KPto8IpfNu6LmP03lnkbRsRHO2Z8-nS2jUcp2eBMn4_hC5lA3iBWWTCadRsXLMT4Uic7PaKfO9XkmmeXZ6i5mOLy9waJusXbNrCAPHRmXlMJQhoOJjti_RkNuMWOScSgtFpIt99g/EEPROM3.png?psid=1&width=412&height=212 "EEPROM type 3")



Compatibility
----------------------
I have tested the program with an Arduino UNO on Debian@3.16.0-4-amd64, but it should work with every POSIX compatible OS.


### TTY device names
- In Mac should be the same as the original `/dev/cu.usbmodem14931` (BUT not tested by me)
- In Linux is `/dev/ttyACM*`, where * should be 0. (you need to be root, or be in [group-mode](http://playground.arduino.cc/Linux/All#Permission))


### Compatible EEPROMs
I've tested with the following devices:
 - GT24C64
 - AT24C32
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
To read 1024bits and save it under the name test.bin and printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128`

To read 64Kbytes and save it under the name large.bin without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 64k -o large.bin -p n`

To read 10Kbytes as text(ascii) and save it under the name readme.txt without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 10k -o large.txt -f a -p n`

To read 1024Kbits from an 25XX1025 and save it without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128k -p n -d 2`

To read 1024Kbits from an 25XX1026 and save it without printing it's content:
 - `./i2c_read_eeprom -t /dev/ttyACM0 -n 128k -p n -d 1`

Afterwards, you can edit the data with your favourite hex editor.



TODO
------------
- Improve C program regarding interrupt signal and clean exit function in case of errors

- Read from a start address to a end address

- Read smaller memory EEPROMs, some only uses 8 bits to 10 bits addressing (sending the device address+MSB and then LSB)



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

