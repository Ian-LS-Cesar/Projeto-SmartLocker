#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include "Wire.h"
#include "I2CKeyPad.h"

int pino_rst = 15;
int pino_sda = 5;
int led_verde = 4;
int led_vermelho = 2;
int buzzer = 33;
int modo = 0;
int segundos = 0;
int trava = 32;
MFRC522 rfid(pino_sda, pino_rst);
byte allowedUID[4] = {0x73, 0x03, 0x5F, 0x09};

String mensagens[] = {"Acesso Aceito", "Acesso Negado"};
int indiceMensagem = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


/* CONFIGURAÇÕES DO WIFI */
const char* ssid = ""; // Nome da rede WiFi
const char* password = ""; // Senha da rede WiFi

/* CONFIGURAÇÕES DO MQTT*/
const char* mqttServer = "broker.hivemq.com"; // Endereço do Broker MQTT
const int mqttPort = 1883; // Porta TCP do Broker MQTT
const char* mqttUser = ""; // Usuário MQTT
const char* mqttPassword = ""; // Senha MQTT

WiFiClient espClient; // Cliente de Rede WiFi
PubSubClient clientMqtt(espClient); // Cria uma instancia de um cliente MQTT

void setup() {

    // Configuração dos pinos do ESP32
    pinMode(led_verde, OUTPUT);
    pinMode(led_vermelho, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(trava, OUTPUT);
    digitalWrite(trava, HIGH);
    
    Serial.begin(9600); // Configura a porta serial
    lcd.init();
    lcd.backlight();
    SPI.begin();
    rfid.PCD_Init();
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
    acessoAutorizado();
  } else {
    acessoNegado();
  }

  delay(1000);



}

void acessoAutorizado(){
  digitalWrite(led_verde, HIGH);
  liquidCrystal(0);
  destrancar();
  delay(1000);
  digitalWrite(led_verde, LOW);
}

void acessoNegado(){
  digitalWrite(led_vermelho, HIGH);
  apitarBuzzer();
  liquidCrystal(1);
  delay(1000);
  digitalWrite(led_vermelho, LOW);
}

void apitarBuzzer(){
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

void liquidCrystal(int indiceMensagem){
  lcd.setCursor(0,0);
  lcd.print(mensagens[indiceMensagem]);
  delay(1000);
  lcd.clear();
}

void destrancar(){
    digitalWrite(trava, LOW);
    delay(5000);
    digitalWrite(trava, HIGH);
}
