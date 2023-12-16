#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Students"
#define WIFI_PASSWORD "Vij2ZtPvT4"

#define API_KEY "AIzaSyCfH7YG9bpzdNBP7_BbPBncfry7qOziKdg"

#define DATABASE_URL "https://espchat-8d969-default-rtdb.europe-west1.firebasedatabase.app/" 

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
String message = "Boyancho: ";
void setup() {
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    //Serial.println("ok");
    signupOK = true;
  }
  else{
    //Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
int y = 0;
int n = 0;
int timePassed = 0;
int pastCount = 0;
String code = "";
bool space = true;
String Dictionary(bool dir, String input){
  String codes[39] = {"._", "_...", "_._.", "_..", ".", ".._.", "__.", "....", "..", ".___", "_._", "._..", "__", "_.", "___", 
                      ".__.", "__._", "._.", "...", "_", ".._", "..._", ".__", "_.._", "_.__", "__..", ".____", "..___", "...__", 
                      "...._", ".....", "_....", "__...", "___..", "____.", "_____", "._._._", "__..__", "..__.."}; 
  String letters[39] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
                      "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", 
                      "4", "5", "6", "7", "8", "9", "0", ".", ",", "?"}; 
  for (int i = 0; i < 39; i++)
  {
    if(dir){
      if (codes[i] == input){
        space = false;
        return letters[i];
      }
    }
    else{
      if (letters[i] == input){
        space = false;
        return codes[i];
      }
    }
  }
  return "";
}
void loop() {
  //Serial.println(digitalRead(1));
  if (timePassed % 50 == 0){
    if (Firebase.RTDB.getInt(&fbdo, "/Messages count")){
      if (fbdo.dataType() == "int"){
        int count = fbdo.intData();
        if (count != pastCount){
          for (int i = pastCount; i < count; i++){
            if (Firebase.RTDB.getString(&fbdo, "/Messages/" + std::to_string(i))){
              if (fbdo.dataType() == "string"){
                Serial.println(fbdo.stringData());
              }
            }
          }
          pastCount = count;
        }
      }
    }
  }
  if(digitalRead(D1)){
    tone(D0, 500);
    y++;
    n = 0;
  }
  else{
    n++;
    if (y > 0){
      if (y < 4){
        code = code + ".";
      }
      else{
        code = code + "_";
      }
      y = 0;
    }
    noTone(D0);
  }
  if (code != ""){
    if (n > 7 && n < 14 ){
      message = message + Dictionary(true, code);
      code = "";
    }
  }
  if (n == 35 && !space){
     message = message + " ";
     space = true;
  }
  AddMessage();
  delay(75);
  timePassed++;
}

void AddMessage(){
  if (digitalRead(D2) && message != "Boyancho: ")
  {
    tone(D0, 500);
    space = true;
    if (Firebase.RTDB.getInt(&fbdo, "/Messages count")){
      if (fbdo.dataType() == "int"){
        Firebase.RTDB.setString(&fbdo, "Messages/" + std::to_string(fbdo.intData()), message);
        Firebase.RTDB.setInt(&fbdo, "Messages count", fbdo.intData() + 1);
        message = "Boyancho: ";
      }
    }
    noTone(D0);
  }
}
