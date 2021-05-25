#include <SPI.h>

#define OUT_PIN  11
#define IN_PIN   12
#define SCLK_PIN 13
#define SS_PIN   10

#define READ_OP 3
#define WREN_OP 6
#define WRITE_OP 2
#define RDSR_OP 5
#define WRSR_OP 1

byte data_from_eeprom = 0;
byte tmp = 0;

byte spi_write(volatile byte data)
{
//  Serial.print("Data: ");
//  Serial.print(data, DEC);
//  Serial.print('\n');
  SPDR = data;
  while(!(SPSR & (1<<SPIF)))
  {
  }
  return SPDR;
}

void setup()
{
  Serial.begin(115200);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, INPUT);
  pinMode(SCLK_PIN, OUTPUT);
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  SPCR = (1<<SPE)|(1<<MSTR);
  tmp = SPSR;
  tmp = SPDR;
  delay(10);
}

byte read_address(int addr)
{
  byte data;
  digitalWrite(SS_PIN, LOW);
  spi_write(READ_OP);
  spi_write((byte)(addr >> 8));
  spi_write((byte)addr);
  data = spi_write(0xFF);
  digitalWrite(SS_PIN, HIGH);
  return data;
}

int address = 0;

void loop()
{
  String command;
  command = Serial.readString();
  if(command[0] == 'r')
  {
//    Serial.println("Starting read");
    /* Extract size */
    int sz = 0;
    String sz_str;
    for(short i = 2; i < command.length(); i++)
      sz_str += command[i];

    sz = sz_str.toInt();
    while(address < sz)
    {
      byte data = read_address(address);
      address++;
      Serial.write(data);
    }
  }
  if(command[0] == 'w')
  {
    /* Extract size */
    int sz = 0;
    String sz_str;
    for(short i = 2; i < command.length(); i++)
      sz_str += command[i];

    sz = sz_str.toInt();
    char to_write[64];
    digitalWrite(SS_PIN, LOW);
    spi_write(WREN_OP);
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SS_PIN, LOW);
    byte tmp = 100;
    tmp = spi_write(RDSR_OP);
    Serial.print("Status reg: ");
    Serial.println(tmp);
    digitalWrite(SS_PIN, HIGH);
    tmp = 0;
    tmp |= 1<<1;
    digitalWrite(SS_PIN, LOW);
    spi_write(WREN_OP);
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SS_PIN, LOW);
    spi_write(WRSR_OP);
    spi_write(tmp);
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SS_PIN, LOW);
    spi_write(WREN_OP);
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SS_PIN, LOW);
    spi_write(WRITE_OP);
    for (int I=0;I<128;I++)
    {
      spi_write(I); //write data byte
    }
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SS_PIN, LOW);
    tmp = spi_write(RDSR_OP);
    digitalWrite(SS_PIN, HIGH);
    Serial.print("Status register after WRSR: ");
    Serial.println(tmp);
  }
#if 0
  if(command[0] == 'w')
  {
    /* Extract size */
    int sz = 0;
    String sz_str;
    for(short i = 2; i < command.length(); i++)
      sz_str += command[i];

    sz = sz_str.toInt();
    char to_write[64];
//    digitalWrite(SS_PIN, LOW);
//    spi_write(WRITE_ENABLE_OPERATION);
//    digitalWrite(SS_PIN, HIGH);
//    delay(300);
    Serial.write(1);
    Serial.setTimeout(1000);
//    Serial.print("WWW");
//    digitalWrite(SS_PIN, LOW);
//    spi_write(WRITE_OPERATION);
//    spi_write((byte)(address>>8));
//    spi_write((byte)address);
    while(address < sz)
    {
      short read_bytes = 0;
     
      while(Serial.available() == 0);
      read_bytes = Serial.readBytes(to_write, 64);
//      Serial.write("Past loop");
//      Serial.print("Got data: ");
//      Serial.write(to_write);
//      Serial.print('\n');
      for(short i = 0; i < read_bytes; i++)
      {
        digitalWrite(SS_PIN, LOW);
        spi_write(WREN_OP);
        //digitalWrite(SS_PIN, HIGH);
        delay(5);
        //digitalWrite(SS_PIN, LOW);
        spi_write(WRITE_OP);
        spi_write((byte)(address>>8));
        spi_write((byte)address);
//        for(int j = 0; j < 3; j++)
//        {
          spi_write(to_write[i]);
//          i++;
//        }
        digitalWrite(SS_PIN, HIGH);
        delay(5);
        digitalWrite(SS_PIN, LOW);
        byte t = spi_write(RDSR_OP);
        digitalWrite(SS_PIN, HIGH);
        Serial.print("Status reg: ");
        Serial.print(t);
      }
      Serial.print("Read bytes: ");
      Serial.print(read_bytes);
      address += read_bytes;
//      Serial.println(address);
//      Serial.write(to_write);
    }
    digitalWrite(SS_PIN, HIGH);
  }
#endif
  address = 0;
}
