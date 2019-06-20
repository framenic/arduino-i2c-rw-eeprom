#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

#include <Wire.h>

#define ADDRESS 0x50

void setup() {
  Serial.begin(115200);

  Wire.begin();
}

void loop() {
  unsigned long num_bytes = 0;
  int dev_type = 0;

  // wait for the integer with the requested number of bytes
  if (Serial.available() == 5) {

    // merge the bytes to single integer to obtain read size
    for(unsigned int i = 0; i < 4; i++)
      num_bytes |=  (((unsigned long) Serial.read()) << (i * 8));

    // read the last byte
    dev_type = Serial.read();   

    switch (dev_type) {
    case 0x01:
      read_small_eeprom(0, num_bytes);
      break;
    case 0x02:
      if(num_bytes <= 65536) //just in case of misunderstading
        read_large_eeprom(ADDRESS, 0, num_bytes);
      else {
        read_large_eeprom(ADDRESS, 0, 65536);
        read_large_eeprom((ADDRESS | 4), 65536, num_bytes);
      }
      break;
    case 0x81:
      write_small_eeprom(0, num_bytes);
      break;
      
    default:
    /*
      if(num_bytes <= 65536) // to read up to 64Kbytes
        read_large_eeprom(ADDRESS, 0, num_bytes);
      else {
        read_large_eeprom(ADDRESS, 0, 65536); // read 64kbytes

        if(num_bytes <= 131072)
          read_large_eeprom((ADDRESS | 1), 65536, num_bytes);
        else {
          read_large_eeprom((ADDRESS | 1), 65536, 131072);

          if(num_bytes <= 196608)
            read_large_eeprom((ADDRESS | 2), 131072, num_bytes);
          else { // WHOLE MEMORY
            read_large_eeprom((ADDRESS | 2), 131072, 196608);
            
            read_large_eeprom((ADDRESS | 3), 196608, num_bytes);
          }
        }
      }
     */ 
      break;
    }
  }
}

void read_large_eeprom(int address, unsigned long first, unsigned long bytes) {
  byte value;
  unsigned long add;

  for(add = first; add < bytes; add++) {
    Wire.beginTransmission(address);
    Wire.write(byte(add >> 8)); //MSB
    Wire.write(byte(add & 0xFF));  //LSB
    Wire.endTransmission();

    //Wire.beginTransmission(address);
    Wire.requestFrom(address, 1);

    if(Wire.available()) value = Wire.read();
    Serial.write(value);
  }
}

void read_small_eeprom(unsigned long first, unsigned long bytes) {
  byte value;
  unsigned long add;

  for(add = first; add < bytes; add++) {
    Wire.beginTransmission(byte(ADDRESS | ((add >> 8) & 0x07)));
    Wire.write(byte(add & 0xFF)); // just LSB matter
    Wire.endTransmission();

    //Wire.beginTransmission(address);
    Wire.requestFrom(byte(ADDRESS | ((add >> 8) & 0x07)), byte(1));

    if(Wire.available()) value = Wire.read();
    Serial.write(value);
  }
}

void write_small_eeprom(unsigned long first, unsigned long bytes) {
  byte value; 
  unsigned long add;

  Serial.setTimeout(10000);
  
  for(add = first; add < bytes; add++) {
    
    Serial.readBytes(&value, 1);
    
    do {
      Wire.beginTransmission(byte(ADDRESS | ((add >> 8) & 0x07)));
      Wire.write(byte(add & 0xFF)); // just LSB matter
      Wire.write(value);
    } 
    while (Wire.endTransmission()!=0);

    Serial.write(byte(add));
    Serial.write(byte(value));
  }

  Serial.setTimeout(1000);  
}

