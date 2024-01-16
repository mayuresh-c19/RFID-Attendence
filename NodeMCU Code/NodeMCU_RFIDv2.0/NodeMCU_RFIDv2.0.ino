
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);



#define SS_PIN  D4  //D4
#define RST_PIN D3  //D3
const int LED = D0;
const int buzzerPin = D8;

MFRC522 mfrc522(SS_PIN, RST_PIN); 


const char *ssid = "mayuresh";
const char *password = "19182003";
const char* device_token  = "3035303933323634";

String URL = "http://192.168.108.71/rfidattendance/getdata.php";
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;

void setup() {
  delay(1000);
  
  Serial.begin(115200);
  SPI.begin();  
  mfrc522.PCD_Init(); 
  //---------------------------------------------
  connectToWiFi();
   pinMode(LED, OUTPUT);
   pinMode(buzzerPin, OUTPUT); 
    lcd.init();   
  lcd.backlight(); 
  lcd.print(" VIT ATTENDANCE "); 

  // digitalWrite(LED, HIGH);
}

void loop() {
  
  if(!WiFi.isConnected()){
    connectToWiFi();    
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }


  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------

  SendCardID(CardID);
  delay(1000);
}

void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
    digitalWrite(D1, LOW);
  if(WiFi.isConnected()){
    WiFiClient client;
    HTTPClient http;
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token);
    //GET methode
    Link = URL + getData;
    http.begin(client, Link); 
    
    int httpCode = http.GET();   
    String payload = http.getString();    

//    Serial.println(Link);   
    Serial.println(httpCode); 
    Serial.println(Card_uid);   
    Serial.println(payload);  
    digitalWrite(LED, HIGH);
    tone(buzzerPin, 100); delay(100); 
    noTone(buzzerPin); 

       lcd.clear();
delay(100);

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
        lcd.setCursor(0,0);
     lcd.print(user_name);
     lcd.setCursor(0,1);  
     lcd.print("LogIn");

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
    //  Serial.println(user_name);
      lcd.setCursor(0,0);
     lcd.print(user_name);
     lcd.setCursor(0,1);  
     lcd.print("LogOut");
  
      }
      else if (payload == "succesful") {
    
      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();  
      delay(500);
        digitalWrite(LED, LOW);
    }
  }
}

void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  
    
    delay(1000);
}
//=======================================================================
