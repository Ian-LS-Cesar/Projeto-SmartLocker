#include <WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include "Wire.h"
#include "I2CKeyPad.h"

const byte ledVerde = 4;
const byte ledVermelho = 2;
void setup() {
  pinMode(ledVerde, OUTPUT);     
  pinMode(ledVermelho, OUTPUT);     
}

void loop() {
  
}