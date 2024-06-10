#define BLYNK_TEMPLATE_ID "TMPL64GaVnobX"
#define BLYNK_TEMPLATE_NAME "AquaGraze"
#define BLYNK_AUTH_TOKEN "mNXHYwkB4phghZ6iqgkM9flcU_m81l0A"
//Define wifi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "PLDTHOMEFIBR9b878";
char pass[] = "PLDTWIFIdbe9z";  

#define BLYNK_DEBUG
#define BLYNK_PRINT Serial
//Define pins
#define SERVOBTN V0
#define LED V1
#define SERVO_OUTPUT V2
#define GREEN_LIGHT 15
#define BUZZER 16
#define SERVO 13


//Libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include "BlynkEdgent.h"
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

int idle_position = 0;
int servo_on = 180;
int servo_off = 0;
String text;
//Virtual pins
int ledPin;
int servoVal;


BLYNK_WRITE(LED){
  ledPin = param.asInt();
    
}
BLYNK_WRITE(SERVOBTN){
  servoVal = param.asInt();
  servo.write(servoVal);
  Blynk.virtualWrite(V2, servoVal);
}
//BLYNK_WRITE(V2){
//  int s1 = param.asInt();
////  Blynk.virtualWrite(V2, idle_position);  
//}

#define APP_DEBUG
#define USE_NODE_MCU_BOARD



void setup()
{
  Serial.begin(115200);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  servo.attach(SERVO); 
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  delay(100);
  lcd.setCursor(0,0);
  lcd.print("Welcome to");
  lcd.setCursor(0,1);
  lcd.print("AquaGraze");
  delay(3000);
}

void loop() {
  Blynk.run();


  if (ledPin == 1 && idle_position != servo_on){
    digitalWrite(GREEN_LIGHT, HIGH);
    lcd.clear();
    text = "Feeding";
    lcd.setCursor(0,0);
    lcd.print("Feeding");
    Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(SERVO);
    servo.write(servo_on);
    Blynk.virtualWrite(V2, servo_on);
    delay(500);
    servo.detach();
    delay(500);
    digitalWrite(BUZZER, LOW);
    idle_position = servo_on; 
//    Serial.print(" ", + idle_position);
  } else if (ledPin == 0 && idle_position != servo_off){ 
    digitalWrite(GREEN_LIGHT, LOW);
    lcd.clear();
    text = "Stop";
    lcd.setCursor(0,0);
    lcd.print("Stop");
    Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(SERVO);
    servo.write(servo_off);
    Blynk.virtualWrite(V2, servo_off);
    delay(500);
    servo.detach();
    delay(500);
    digitalWrite(BUZZER, LOW);
    idle_position = servo_off;
//    Serial.print(" ", + idle_position);
  }


}
