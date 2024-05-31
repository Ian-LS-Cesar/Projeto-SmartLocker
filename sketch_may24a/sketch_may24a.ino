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

String mensagens[] = {"Acesso Aceito", "Acesso Negado"};
int indiceMensagem = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


/* CONFIGURAÇÕES DO WIFI */
const char* ssid = "iPhone de Ian Lucas"; // Nome da rede WiFi
const char* password = "321321321"; // Senha da rede WiFi

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
    
}

void loop() {
  destrancar();
  trancar();
}

void acessoAutorizado(){
  digitalWrite(led_verde, HIGH);
  liquidCrystal(0);
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
  delay(1000);
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
    delay(3000);
}

void trancar(){
  digitalWrite(trava, HIGH);
  delay(3000);
}