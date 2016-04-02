#define ADDRESS 0x50

#include <Wire.h>

byte byte1, byte2;
int addressFinal;
unsigned long num_bytes;

void setup() {
  Serial.begin(115200);

  Wire.begin();
}

void loop() {
  int dev_type = 0;
  int address;

  // wait for the integer with the requested number of bytes
  if (Serial.available() == 5) {
    num_bytes = 0;

    // merge the bytes to single integer to obtain read size
    for(unsigned int i = 0; i < 4; i++)
      num_bytes |=  (((unsigned long) Serial.read()) << (i * 8));

    // read the last byte
    dev_type = Serial.read();

    if(dev_type == 0)
      read_eeprom(ADDRESS, 0, num_bytes);
    else {
      // setup the address var
      address = ADDRESS;

      // first read 64kbytes
      read_eeprom(ADDRESS, 0, 65536);

      switch (dev_type) {
      case 1:
        address |= (1 << 0);
        read_eeprom(address, 65536, num_bytes);
        break;
      case 2:
        address |= (1 << 2);
        read_eeprom(address, 65536, num_bytes);
        break;
      case 3:
        address |= (1 << 0);

        if(num_bytes <= 131072)
          read_eeprom(address, 65536, num_bytes);      
        else {
          read_eeprom(address, 65536, 131072);

          // remove A16
          address |= (0 << 0);

          // set A17 to read upper memory
          address |= (1 << 1);

          if(num_bytes <= 196608)
            read_eeprom(address, 131072, num_bytes);

          else {  // WHOLE MEMORY
            read_eeprom(address, 131072, 196608);

            // set again A16
            address |= (1 << 0);
            read_eeprom(address, 196608, num_bytes);
          }
        }
        break;
      default:
        // ... waht to do ?
		// send over 0xFF
        break;
      }
    }
  }

}


void read_eeprom(int address, unsigned long first, unsigned long bytes) {
  byte value;
  unsigned long add;

  for(add = first; add < bytes; add++) {
    Wire.beginTransmission(address);
    Wire.write(highAddressByte(add)); //MSB
    Wire.write(lowAddressByte(add));  //LSB
    Wire.endTransmission();

    Wire.beginTransmission(address);
    Wire.requestFrom(address, 1);

    if (Wire.available()) value = Wire.read();
    Serial.write(value);
  }
}

byte highAddressByte(int add) {
  byte1 = (add >> 8);
  return byte1;
}

byte lowAddressByte(int add) {
  byte1 = (add >> 8);
  byte2 = (add - (byte1 << 8));
  return byte2;
}

