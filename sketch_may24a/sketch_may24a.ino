#include "Wire.h"
#include "I2CKeyPad.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

I2CKeyPad teclado(0x20);

int ultima_vez_apertado = 0;

int pino_rst = 15;
int pino_sda = 5;
int led_verde = 2;
int led_vermelho = 4;
int buzzer = 33;
int trava = 32;
MFRC522 rfid(pino_sda, pino_rst);
byte allowedUID[4] = { 0x13, 0x66, 0x6F, 0x09 };

String mensagens[] = { "Acesso Aceito", "Acesso Negado" };
int indiceMensagem = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "iPhone de Ian Lucas";
const char* password = "321321321";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient clientMqtt(espClient);

void setup() {
  Serial.begin(9600);

  Wire.begin();
  Wire.setClock(400000);
  if (!teclado.begin()) {
    Serial.println("Falha na comunicação do Teclado. Reinicie o dispositivo.");
    while (1)
      ;
  }

  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trava, OUTPUT);
  digitalWrite(trava, HIGH);

  lcd.init();
  lcd.backlight();
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Aproxime o cartão ou chaveiro RFID ao leitor...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Tentando se conectar na rede: ");
    Serial.println(ssid);
  }

  Serial.println("Conectado na Rede WiFi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  clientMqtt.setServer(mqttServer, mqttPort);
  clientMqtt.setCallback(callback);

  while (!clientMqtt.connected()) {
    Serial.println("Conectando ao Broker MQTT...");
    String clientId = "ESP32Client_" + String(random(0xffff), HEX);
    Serial.println("clientId = " + clientId);
    if (clientMqtt.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("O cliente " + clientId + " foi conectado com sucesso");
    } else {
      int clientState = clientMqtt.state();
      Serial.print("Falha ao se conectar. ");
      Serial.println("Estado do cliente: " + (String)clientState);
      delay(2000);
    }
  }

  Serial.print("Tentando enviar a mensagem");
  clientMqtt.publish("ian/smartlocker", "ESP32 Conectado");
  clientMqtt.subscribe("ian/smartlocker");
}

int lastState = -1;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  int newState = (payload[0] == '0');
  if (newState != lastState) {
    lastState = newState;
    if (newState == 0) {
      Serial.println("Trava ON");
      digitalWrite(trava, LOW);
    } else {
      Serial.println("Trava OFF");
      digitalWrite(trava, HIGH);
    }
  }
}

String code = "";  // global variable to store the 4-digit code


void loop() {

  clientMqtt.loop();


  int tempo_atual = millis();

  char teclas[] = "123A456B789C*0#DNF";

  int indice = teclado.getKey();


  if (teclado.isPressed()) {

    char tecla = teclas[indice];

    if (tempo_atual - ultima_vez_apertado >= 200) {

      ultima_vez_apertado = millis();

      Serial.println(tecla);

      code += tecla;
      if (code.length() == 4) {

        if (code == "1111") {  // change this to your desired 4-digit password
          acessoAutorizado();

        } else {
          acessoNegado();
        }

        code = "";  // reset code
      }
    }
  }


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

void acessoAutorizado() {
  Serial.println("Acesso Autorizado!!!");
  digitalWrite(led_verde, HIGH);
  apitarBuzzer(0);
  liquidCrystal(0);
  destrancar();
  delay(1000);
  digitalWrite(led_verde, LOW);
}

void acessoNegado() {
  Serial.println("Acesso Negado!!!");
  digitalWrite(led_vermelho, HIGH);
  apitarBuzzer(1);
  liquidCrystal(1);
  delay(1000);
  digitalWrite(led_vermelho, LOW);
  digitalWrite(trava, HIGH);
}

void apitarBuzzer(int indiceMensagem) {
  if (indiceMensagem == 0) {
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
  } else {
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
  }
}

void liquidCrystal(int indiceMensagem) {
  lcd.setCursor(0, 0);
  lcd.print(mensagens[indiceMensagem]);
  delay(1500);
  lcd.clear();
}

void destrancar() {
  digitalWrite(trava, LOW);
  delay(5000);
  digitalWrite(trava, HIGH);
}