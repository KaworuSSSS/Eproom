#include <SPI.h>

#define CS_SRC 5
#define CS_DST 4
#define EEPROM_SIZE 8192

void writeEnable(uint8_t cs) {
  digitalWrite(cs, LOW);
  SPI.transfer(0x06);
  digitalWrite(cs, HIGH);
}

byte readByte(uint8_t cs, unsigned int addr) {
  digitalWrite(cs, LOW);
  SPI.transfer(0x03);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);
  byte data = SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  return data;
}

void writeByte(uint8_t cs, unsigned int addr, byte data) {
  writeEnable(cs);
  digitalWrite(cs, LOW);
  SPI.transfer(0x02);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);
  SPI.transfer(data);
  digitalWrite(cs, HIGH);
  delay(5);
}

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23);

  pinMode(CS_SRC, OUTPUT);
  pinMode(CS_DST, OUTPUT);

  digitalWrite(CS_SRC, HIGH);
  digitalWrite(CS_DST, HIGH);

  Serial.println("Copiando EEPROM byte a byte...");

  for (unsigned int i = 0; i < EEPROM_SIZE; i++) {
    byte b = readByte(CS_SRC, i);
    writeByte(CS_DST, i, b);
    
  }

  Serial.println("Copia completa.");
}

void loop() {}
