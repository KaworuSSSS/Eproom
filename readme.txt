📌 1. Pines SPI del ESP32 (bus SPI por defecto)
Señal	GPIO ESP32
MOSI	23
MISO	19
SCK	18
GND	GND
3.3V	3.3V

⚠ Las 25LC640 funcionan a 3.3 V, así que no necesitas resistencias ni conversores de nivel.

📌 2. Pines de la EEPROM 25LC640 (SOIC/PDIP de 8 pines)
      25LC640
   ┌──────────────┐
1 -| CS       VCC |- 8 (3.3V)
2 -| SO       HOLD|- 7 (poner a 3.3V)
3 -| WP        SCK|- 6 (a GPIO 18)
4 -| GND       SI |- 5 (a GPIO 23)
   └──────────────┘


Importante:

SO (pin 2) = MISO → a GPIO 19

SI (pin 5) = MOSI → a GPIO 23

SCK (pin 6) → a GPIO 18

CS (pin 1) → cada EEPROM debe tener su propio pin CS

WP (pin 3) y HOLD (pin 7) se deben poner a 3.3 V (con resistencias o directos)

📌 3. Conexión de las dos EEPROM
📍 EEPROM origen (la que vas a leer)

CS → GPIO 5

SO → GPIO 19

SI → GPIO 23

SCK → GPIO 18

WP → 3.3V

HOLD → 3.3V

VCC → 3.3V

GND → GND

📍 EEPROM destino (la que vas a escribir)

CS → GPIO 4

SO → GPIO 19 (compartido)

SI → GPIO 23 (compartido)

SCK → GPIO 18 (compartido)

WP → 3.3V

HOLD → 3.3V

VCC → 3.3V

GND → GND

🧩 Diagrama resumido tipo "mapa"
ESP32          EEPROM Origen      EEPROM Destino
--------       -------------      ----------------
GPIO 18 -----> SCK (6)           SCK (6)

GPIO 23 -----> SI (5)            SI (5)

GPIO 19 <----- SO (2)            SO (2)

3.3V --------> VCC (8)           VCC (8)
3.3V --------> WP (3)            WP (3)
3.3V --------> HOLD (7)          HOLD (7)

GND ----------> GND (4)          GND (4)

GPIO 5 ------> CS (1)  ← Origen
GPIO 4 ------> CS (1)  ← Destino

📌 Notas importantes
✔ Ambas EEPROM comparten:

MISO (SO)

MOSI (SI)

SCK

GND

3.3V

✔ Solo el pin CS cambia

Esto permite que la ESP32 seleccione cuál EEPROM quiere leer/escribir.

✔ HOLD y WP siempre en 3.3V

Si los dejas flotando la EEPROM puede no funcionar.

VERSIÓN SIMPLE (BYTE POR BYTE)

(Lo que usabas, ajustado al tamaño correcto)

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
