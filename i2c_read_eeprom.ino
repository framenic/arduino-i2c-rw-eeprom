/* 
 Number  Name   ConnectTo
 1       A0     GND
 2       A1     GND
 3       A2     GND/Vcc (read datasheet)
 4       GND    GND
 5       SDA    SDA
 6       SCL    SCL
 7       WP     GND
 8       VCC    (+3v3 ... +5V0)
 */

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

  // wait for the integer with the requested number of bytes
  if (Serial.available() == 7) {
    num_bytes = 0;

    // merge the bytes to single integer
    for(unsigned int i = 0; i < 7; i++)
      num_bytes |=  (((unsigned long) Serial.read()) << (i * 8));

    /*
    if(num_bytes >= 64k)
     fazer 2 read_eeprom sendo uma no segundo bloco
     fazer a mudanca no ADDRESS ou dependendo do metodo
     */

    read_eeprom(num_bytes);
  }
}


void read_eeprom(unsigned long bytes) {
  byte value;
  unsigned long add;

  for(add = 0; add < bytes; add++) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(highAddressByte(add)); //MSB
    Wire.write(lowAddressByte(add));  //LSB
    Wire.endTransmission();

    Wire.beginTransmission(ADDRESS);
    Wire.requestFrom(ADDRESS, 1);

    //Wait till we get all the bytes
    //while ( Wire.available() < 1) {
    //};

    if (Wire.available()) value = Wire.read();
    Serial.write(value);
  }
}

byte highAddressByte(word add) {
  byte1 = (add >> 8);
  return byte1;
}

byte lowAddressByte(word add) {
  byte1 = (add >> 8);
  byte2 = (add - (byte1 << 8));
  return byte2;
}

