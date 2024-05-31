#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 15
#define ACCESS_GRANTED_LED 2 // LED para acesso permitido

MFRC522 rfid(SS_PIN, RST_PIN);

byte allowedUID[4] = {0x73, 0x03, 0x5F, 0x09}; // Substitua pelo UID do seu cartão

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(ACCESS_GRANTED_LED, OUTPUT);
  digitalWrite(ACCESS_GRANTED_LED, LOW);
  Serial.println("Aproxime o cartão ou chaveiro RFID ao leitor...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
 
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
 
  bool accessGranted = true;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      accessGranted = false;
      break;
    }
  }

  if (accessGranted) {
    Serial.println("Acesso Permitido!");
    digitalWrite(ACCESS_GRANTED_LED, HIGH);
  } else {
    Serial.println("Acesso Negado!");
    digitalWrite(ACCESS_GRANTED_LED, LOW);
  }

  delay(1000);
  digitalWrite(ACCESS_GRANTED_LED, LOW);
}