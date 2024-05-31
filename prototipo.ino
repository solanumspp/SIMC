#include <WifiLocation.h>
#include <bingMapsGeocoding.h>
#include <UnixTime.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define sensor 23
#define token_acesso_telegram "TOKEN_TELEGRAM"
#define id_chat "ID"
#define chaveGoogleAPI "TOKEN_GOOGLE_API"

LiquidCrystal_I2C lcd(0x27,16,2);
WiFiClientSecure cliente_seguro;
UniversalTelegramBot bot(token_acesso_telegram, cliente_seguro);

const char* ssid = "JESSICA FIBRA 2.4";
const char* senha =  "Jf1702*#";
int estado;
int janelaDeteccao = 3000;
unsigned long timerAnterior = 0, timerAtual = 0;
location_t loc;
UnixTime timestamp(-4);
WifiLocation local(chaveGoogleAPI);

void setup(){
  Serial.begin(115200);

  pinMode(sensor, INPUT_PULLUP);
  Serial.flush();

  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, senha);
  cliente_seguro.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    lcd.setCursor(0, 0);
    lcd.print("Autenticando...");
    Serial.println("Autenticando...");
  }
  lcd.print("Conectado!");
  Serial.println("Conectado!");
  printWifiData();

  Serial.print("Obtendo data e hora: ");
  configTime(-14400, 0, "pool.ntp.br");
  Serial.println(
    String(timestamp.day) + 
    "/" + 
    String(timestamp.month) + 
    "/" + 
    String(timestamp.year) + 
    " - " + 
    String(timestamp.hour) + 
    ":" + 
    String(timestamp.minute) + 
    ":" + 
    String(timestamp.second)
  );

  loc = local.getGeoFromWiFi();

  Serial.println("Latitude: " + String(loc.lat, 7));
  Serial.println("Longitude: " + String(loc.lon, 7));
  Serial.println("Precisão: " + String(loc.accuracy));

  atualizaTela();
}

void loop(){
  delay(10);
  
  timerAtual = millis();
  
  estado = digitalRead(sensor);

  if(estado == LOW){
    time_t agora = time(nullptr);
    timestamp.getDateTime(agora);
    bot.sendMessage(
      id_chat, 
      String(timestamp.day) + 
      "/" + 
      String(timestamp.month) + 
      "/" + 
      String(timestamp.year) + 
      " - " + 
      String(timestamp.hour) + 
      ":" + 
      String(timestamp.minute) + 
      ":" + 
      String(timestamp.second) + 
      " - Foco de calor detectado! - " + 
      "Lat: " + String(loc.lat, 7) +
      ", " +
      "Lon: " + String(loc.lon, 7) +
      ", " +
      "Precisão: " + String(loc.accuracy), 
      ""
    );
    atualizaTela();
  }

  atualizaTela();
}

void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("Endereço IP: ");
  Serial.println(ip);

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("Endereço MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void atualizaTela(){
  lcd.clear();
  lcd.setCursor(0,0);

  if(estado == LOW){
    if(timerAtual - timerAnterior >= janelaDeteccao){
      if(estado == LOW){
        lcd.print("Alerta: foco de");
        lcd.setCursor(0, 1);
        lcd.print("calor detectado!");
      }
    }
  }
  else{
    lcd.print("Sem focos de");
    lcd.setCursor(0,1);
    lcd.print("calor detectados");
  }
}
