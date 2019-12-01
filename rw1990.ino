#include <OneWire.h>
#define vcc 11
#define pin 12

OneWire ibutton (pin); // I button connected on PIN 2.

byte addr[8]; //array to store the Ibutton ID.

// Hardcode here your desired ID
// 01 23 45 67 89 AB CD EF
byte newID[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

int writeBit(bool data)
{
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
  if(data) delayMicroseconds(10);
    else delayMicroseconds(60);
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
  delay(10);
}

int writeByte(byte data)
{
  int data_bit;
  for(data_bit=0; data_bit<8; data_bit++)
  {
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    if(data & 1) delayMicroseconds(60);
      else delayMicroseconds(10);
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
    delay(10);
    data = data >> 1;
  }
  return 0;
}

void setup()
{
 Serial.begin(9600);
 
 pinMode(vcc, OUTPUT);
 digitalWrite(vcc, HIGH);
}

void loop()
{
  if (!ibutton.search (addr))//read attached ibutton and asign value to buffer
  {
    ibutton.reset_search();
    delay(200);
    return;
  }
 
  Serial.print(millis()/1000);
  Serial.print("> ");
  for (byte x = 0; x<8; x++)
  {  
    Serial.print(addr[x],HEX); //print the buffer content in LSB. For MSB: for (int x = 8; x>0; x--) 
    Serial.print(" ");
  }
 
  //compute crc//
  byte crc;
  crc = ibutton.crc8(addr, 7);
  Serial.print("CRC: ");
  Serial.println(crc,HEX);
  if ( Serial.read() == 'w' )
  {
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0x33);
    Serial.print("  ID before write:");
    for (byte x=0; x<8; x++)
    {
      Serial.print(' ');
      Serial.print(ibutton.read(), HEX);
    }
    
    // send reset
    ibutton.skip();
    ibutton.reset();
    
    // send 0xD1
    ibutton.write(0xD1);
    
    // send logical 0
    writeBit(0);
    
    Serial.print('\n');
    Serial.print("  Writing iButton ID:\n    ");
                     
    // send reset
    ibutton.skip();
    ibutton.reset();

    // send 0xD5
    ibutton.write(0xD5);
    
    for (byte x = 0; x<8; x++)
    {
      writeByte(newID[x]);
      Serial.print('*');
    }
    Serial.print('\n');
    
    ibutton.reset();
    
    // send 0xD1
    ibutton.write(0xD1);
    
    //send logical 1
    writeBit(1);
  }
} 
