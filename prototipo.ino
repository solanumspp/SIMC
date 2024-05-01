#include <Arduino.h>
#include <MQ9.h>
#include <DallasTemperature.h>
#include <WiFi.h> // Inclua se usar ESP8266
#include <LiquidCrystal.h> // Inclua se usar LCD

// Definições de pino
const int pinMQ9 = D2; // Pino do sensor MQ-9
const int pinDS18B20 = D3; // Pino do sensor DS18B20

// Definições do sensor MQ-9
MQ9 mq9(pinMQ9);

// Definições do sensor DS18B20
DallasTemperature sensors(pinDS18B20);
DeviceAddress deviceAddress;

// Definições do Wi-Fi
const char* ssid = "SEU_SSID"; // Substitua pelo seu SSID
const char* password = "SEU_PASSWORD"; // Substitua pela sua senha

// Definições do LCD 
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Variáveis para armazenar valores
float ppmCO;
float temperatureC;

// Limites para determinar risco de incêndio
float coThreshold = 200; // Limite de CO para indicar risco alto
float temperatureThreshold = 50; // Limite de temperatura para indicar risco alto

// Variáveis para armazenar status de risco
bool fireRiskCO = false;
bool fireRiskTemperature = false;

// Função para ler o valor do sensor MQ-9 e converter em ppm de CO
float readMQ9PPM() {
  return mq9.calculatePPM();
}

// Função para ler a temperatura do sensor DS18B20
float readDS18B20Temperature() {
  sensors.requestTemperatures(); // Solicita temperaturas de todos os dispositivos
  return sensors.getTempC(deviceAddress); // Obtem a temperatura do primeiro dispositivo
}

// Função para verificar o risco de incêndio com base no CO
void checkFireRiskCO() {
  ppmCO = readMQ9PPM();
  if (ppmCO >= coThreshold) {
    fireRiskCO = true;
    Serial.println("Risco alto de incêndio devido ao CO!");
  } else {
    fireRiskCO = false;
  }
}

// Função para verificar o risco de incêndio com base na temperatura
void checkFireRiskTemperature() {
  temperatureC = readDS18B20Temperature();
  if (temperatureC >= temperatureThreshold) {
    fireRiskTemperature = true;
    Serial.println("Risco alto de incêndio devido à temperatura!");
  } else {
    fireRiskTemperature = false;
  }
}
// Função para definir o alerta de incêndio
void sendFireAlert() {
  // Envie um alerta via Wi-Fi 
  if (WiFi.isConnected()) {
    // Envie email, SMS ou notificação usando bibliotecas apropriadas (não implementadas)
    Serial.println("Alerta de incêndio enviado via Wi-Fi!");
  } else {
    Serial.println("Falha ao enviar alerta via Wi-Fi. Verifique a conexão!");
  }

  // Exiba o alerta no LCD 
  if (lcd.begin(16, 2)) {
    lcd.print("ALERTA DE INCÊNDIO!");
    lcd.setCursor(0, 1);
    lcd.print("CO: ");
    lcd.print(ppmCO);
    lcd.print(" ppm, Temp: ");
    lcd.print(temperatureC);
    lcd.print(" C");
  } else {
    Serial.println("Falha ao inicializar o LCD!");
  }

  // Ative uma sirene ou outro dispositivo de aviso 
  // ... (não implementado)
}
