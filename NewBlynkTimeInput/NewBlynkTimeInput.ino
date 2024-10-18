#define BLYNK_TEMPLATE_ID "TMPL64GaVnobX"
#define BLYNK_TEMPLATE_NAME "AquaGraze"
// #define BLYNK_AUTH_TOKEN "mNXHYwkB4phghZ6iqgkM9flcU_m81l0A"
// Define wifi credentials
// char auth[] = BLYNK_AUTH_TOKEN;
// char ssid[] = "PLDTHOMEFIBR9b878";
// char pass[] = "PLDTWIFIdbe9z";

#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#define BLYNK_DEBUG
#define BLYNK_PRINT Serial
// Define pins
#define SERVOBTN V0
#define LED V1
#define LCD V5
#define SERVO_OUTPUT V2
#define GREEN_LIGHT 15
#define BUZZER 16
#define SERVO 13
// #define TIME_SCHEDULE V3 //added

// Libraries
// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <ESP8266WebServer.h>
// #include <EEPROM.h>
// #include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BlynkEdgent.h"
#include <Servo.h>
#include <TimeLib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
BlynkTimer timer;
// Establishing Local server at port 80 whenever required
// ESP8266WebServer server(80);

// for wifi configuration
// int i = 0;
// int statusCode;
// const char* ssid = "Default_SSID";
// const char* passphrase = "Default_Password";
// String st;
// String content;
//  Function Declaration
// bool testWifi(void);
// void launchWeb(void);
// void setupAP(void);

// var user in timer----------------//
bool led_set[2];
long timer_start_set[2] = {0xFFFF, 0xFFFF};
long timer_stop_set[2] = {0xFFFF, 0xFFFF};
unsigned char weekday_set[2];

long rtc_sec;
unsigned char day_of_week;

bool led_status[2];
bool update_blynk_status[2];
bool led_timer_on_set[2];
//---------------------------------//

int idle_position = 0;
int servo_on = 180;
int servo_off = 0;
String text;
// Virtual pins
int ledPin;
int servoVal;

// BLYNK Write function
BLYNK_WRITE(LED)
{
  int val = param.asInt();

  if (led_timer_on_set[1] == 0)
    led_set[1] = val;
  Blynk.virtualWrite(LCD, "Button Pressed");
  else update_blynk_status[1] = 1;
  Blynk.virtualWrite(LCD, "Waiting...");
}
BLYNK_WRITE(SERVOBTN)
{
  servoVal = param.asInt();
  servo.write(servoVal);
  Blynk.virtualWrite(SERVO_OUTPUT, servoVal);
}
// Timer 1
BLYNK_WRITE(V3)
{
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime())
  {
    timer_start_set[0] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60) + t.getStartSecond();
    timer_stop_set[0] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60) + t.getStopSecond();

    Serial.println(String("Start Time: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());

    Serial.println(String("Stop Time: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());

    for (int i = 1; i <= 7; i++)
    {
      if (t.isWeekdaySelected(i))
      {
        week_day |= (0x01 << (i - 1));
        Serial.println(String("Day ") + i + " is selected");
      }
      else
      {
        week_day &= (~(0x01 << (i - 1)));
      }
    }

    weekday_set[0] = week_day;
  }
  else
  {
    timer_start_set[0] = 0xFFFF;
    timer_stop_set[0] = 0xFFFF;
  }
}
// Timer 2
BLYNK_WRITE(V4)
{
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime())
  {
    timer_start_set[1] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60) + t.getStartSecond();
    timer_stop_set[1] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60) + t.getStopSecond();

    Serial.println(String("Start Time: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());

    Serial.println(String("Stop Time: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());

    for (int i = 1; i <= 7; i++)
    {
      if (t.isWeekdaySelected(i))
      {
        week_day |= (0x01 << (i - 1));
        Serial.println(String("Day ") + i + " is selected");
      }
      else
      {
        week_day &= (~(0x01 << (i - 1)));
      }
    }

    weekday_set[1] = week_day;
  }
  else
  {
    timer_start_set[1] = 0xFFFF;
    timer_stop_set[1] = 0xFFFF;
  }
}

//--------------------------------------------//
BLYNK_WRITE(InternalPinRTC)
{
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  unsigned long blynkTime = param.asLong();

  if (blynkTime >= DEFAULT_TIME)
  {
    setTime(blynkTime);

    day_of_week = weekday();

    if (day_of_week == 1)
      day_of_week = 7;
    else
      day_of_week -= 1;

    rtc_sec = (hour() * 60 * 60) + (minute() * 60) + second();

    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute() + ":" + second());
    Serial.println(String("Day of Week: ") + weekday());
  }
}
//------------------------------------------//
BLYNK_WRITE(TIME_SCHEDULE)
{
  long setTime = param[0].asLong();
  Serial.println(setTime);
  Serial.println("");
}
// BLYNK_WRITE(V2){
//   int s1 = param.asInt();
////  Blynk.virtualWrite(V2, idle_position);
//}
BLYNK_CONNECTED()
{
  Blynk.sendInternal("rtc", "sync");
}

//-----------------------//
void checkTime()
{
  Blynk.sendInternal("rtc", "sync");
}
//----------------------//
void led_mng()
{
  bool time_set_overflow;
  bool led_status_buf[2];

  for (int i = 0; i < 2; i++)
  {
    led_status_buf[i] = led_status[i];
    time_set_overflow = 0;

    if (timer_start_set[i] != 0xFFFF && timer_stop_set[i] != 0xFFFF)
    {
      if (timer_stop_set[i] < timer_start_set[i])
        time_set_overflow = 1;

      if ((((time_set_overflow == 0 && (rtc_sec >= timer_start_set[i]) && (rtc_sec < timer_stop_set[i])) ||
            (time_set_overflow && ((rtc_sec >= timer_start_set[i]) || (rtc_sec < timer_stop_set[i])))) &&
           (weekday_set[i] == 0x00 || (weekday_set[i] & (0x01 << (day_of_week - 1))))))
      {
        led_timer_on_set[i] = 1;
      }
      else
        led_timer_on_set[i] = 0;
    }
    else
      led_timer_on_set[i] = 0;

    if (led_timer_on_set[i])
    {
      led_status[i] = 1;
      led_set[i] = 0;
    }
    else
    {
      led_status[i] = led_set[i];
    }

    if (led_status_buf[i] != led_status[i])
      update_blynk_status[i] = 1;
  }
  // HARDWARE CONTROL
  //  digitalWrite(15, led_status[0]);
  digitalWrite(GREEN_LIGHT, led_status[1]);
}
//------------------------------------//
void blynk_update()
{
  if (update_blynk_status[0])
  {
    update_blynk_status[0] = 0;
    Blynk.virtualWrite(V0, led_status[0]);
  }

  if (update_blynk_status[1])
  {
    update_blynk_status[1] = 0;
    Blynk.virtualWrite(V1, led_status[1]);
  }
}
//------------------------------------------------//
void openLid()
{
  if (led_timer_on_set[1] == 1 && idle_position != servo_on)
  {
    digitalWrite(GREEN_LIGHT, HIGH);
    lcd.clear();
    text = "Feeding";
    lcd.setCursor(0, 0);
    lcd.print("Feeding");
    Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(13);
    servo.write(servo_on);
    Blynk.virtualWrite(V2, servo_on);
    delay(500);
    servo.detach();
    delay(500);
    digitalWrite(BUZZER, LOW);
    idle_position = servo_on;
    //    Serial.print(" ", + idle_position);
  }
  else if (led_timer_on_set[1] == 0 && idle_position != servo_off)
  {
    digitalWrite(GREEN_LIGHT, LOW);
    lcd.clear();
    text = "Stop";
    lcd.setCursor(0, 0);
    lcd.print("Stop");
    Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(13);
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
//  ------------------------------------------------------------//
void setup()
{
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  servo.attach(13);
  //  setup_wifi();
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("a network");
  BlynkEdgent.begin();
  timer.setInterval(10000L, checkTime);
  Wire.begin();
  //  Blynk.begin(auth, ssid, passphrase);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("AquaGraze");
  delay(3000);
}

void loop()
{
  BlynkEdgent.run();
  timer.run();
  led_mng();
  blynk_update();
  openLid();
}

// void setup_wifi(){
//
//    //Initialising if(DEBUG)Serial Monitor
//   Serial.println();
//   Serial.println("Disconnecting current wifi connection");
//   WiFi.disconnect();
//   EEPROM.begin(512); //Initialasing EEPROM
//   delay(10);
//   pinMode(LED_BUILTIN, OUTPUT);
//   Serial.println();
//   Serial.println();
//   Serial.println("Startup");
//   //---------------------------------------- Read eeprom for ssid and pass
//   Serial.println("Reading EEPROM ssid");
//   String esid;
//   for (int i = 0; i < 32; ++i)
//   {
//     esid += char(EEPROM.read(i));
//   }
//   Serial.println();
//   Serial.print("SSID: ");
//   Serial.println(esid);
//   Serial.println("Reading EEPROM pass");
//   String epass = "";
//   for (int i = 32; i < 96; ++i)
//   {
//     epass += char(EEPROM.read(i));
//   }
//   Serial.print("PASS: ");
//   Serial.println(epass);
//   WiFi.begin(esid.c_str(), epass.c_str());
//   if (testWifi())
//   {
//     Serial.println("Succesfully Connected!!!");
//     return;
//   }
//   else
//   {
//     Serial.println("Turning the HotSpot On");
//     launchWeb();
//     setupAP();// Setup HotSpot
//   }
//   Serial.println();
//   Serial.println("Waiting.");
//   while ((WiFi.status() != WL_CONNECTED))
//   {
//     Serial.print(".");
//     delay(3000);
//     server.handleClient();
//   }
//   }
//
////----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change
// bool testWifi(void)
//{
//   int c = 0;
//   Serial.println("Waiting for Wifi to connect");
//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("Waiting for Wifi");
//   lcd.setCursor(0,1);
//   lcd.print("to connect");
//   while ( c < 20 ) {
//     if (WiFi.status() == WL_CONNECTED)
//     {
//       return true;
//     }
//     delay(3000);
//     Serial.print("*");
//     c++;
//   }
//   Serial.println("");
//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("Can't connect");
//   lcd.setCursor(0,1);
//   lcd.print("to a network");
//   Serial.println("Connect timed out, opening AP"); // add LCD display
//   return false;
// }
//
//   void launchWeb()
//{
//   Serial.println("");
//   if (WiFi.status() == WL_CONNECTED)
//     Serial.println("WiFi connected");
//   Serial.print("Local IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.print("SoftAP IP: ");
//   Serial.println(WiFi.softAPIP());
//   createWebServer();
//   // Start the server
//   server.begin();
//   Serial.println("Server started");
// }
// void setupAP(void)
//{
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect();
//   delay(100);
//   int n = WiFi.scanNetworks();
//   Serial.println("scan done");
//   if (n == 0)
//     Serial.println("no networks found");
//   else
//   {
//     Serial.print(n);
//     Serial.println(" networks found");
//     for (int i = 0; i < n; ++i)
//     {
//       // Print SSID and RSSI for each network found
//       Serial.print(i + 1);
//       Serial.print(": ");
//       Serial.print(WiFi.SSID(i));
//       Serial.print(" (");
//       Serial.print(WiFi.RSSI(i));
//       Serial.print(")");
//       Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
//       delay(10);
//     }
//   }
//   Serial.println("");
//   st = "<ol>";
//   for (int i = 0; i < n; ++i)
//   {
//     // Print SSID and RSSI for each network found
//     st += "<li>";
//     st += WiFi.SSID(i);
//     st += " (";
//     st += WiFi.RSSI(i);
//     st += ")";
//     st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
//     st += "</li>";
//   }
//   st += "</ol>";
//   delay(100);
//   WiFi.softAP("ESP8266_Hotspot", "");
//   Serial.println("Initializing_softap_for_wifi credentials_modification");
//   launchWeb();
//   Serial.println("over");
// }
//
// void createWebServer()
//{
//   {
//     server.on("/", []() {
//       IPAddress ip = WiFi.softAPIP();
//       String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
//       content = "<!DOCTYPE HTML>\r\n<html>Welcome to Wifi Credentials Update page";
//       content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
//       content += ipStr;
//       content += "<p>";
//       content += st;
//       content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
//       content += "</html>";
//       server.send(200, "text/html", content);
//     });
//     server.on("/scan", []() {
//       //setupAP();
//       IPAddress ip = WiFi.softAPIP();
//       String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
//       content = "<!DOCTYPE HTML>\r\n<html>go back";
//       server.send(200, "text/html", content);
//     });
//     server.on("/setting", []() {
//       String qsid = server.arg("ssid");
//       String qpass = server.arg("pass");
//       if (qsid.length() > 0 && qpass.length() > 0) {
//         Serial.println("clearing eeprom");
//         for (int i = 0; i < 96; ++i) {
//           EEPROM.write(i, 0);
//         }
//         Serial.println(qsid);
//         Serial.println("");
//         Serial.println(qpass);
//         Serial.println("");
//         Serial.println("writing eeprom ssid:");
//         for (int i = 0; i < qsid.length(); ++i)
//         {
//           EEPROM.write(i, qsid[i]);
//           Serial.print("Wrote: ");
//           Serial.println(qsid[i]);
//         }
//         Serial.println("writing eeprom pass:");
//         for (int i = 0; i < qpass.length(); ++i)
//         {
//           EEPROM.write(32 + i, qpass[i]);
//           Serial.print("Wrote: ");
//           Serial.println(qpass[i]);
//         }
//         EEPROM.commit();
//         content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
//         statusCode = 200;
//         ESP.reset();
//       } else {
//         content = "{\"Error\":\"404 not found\"}";
//         statusCode = 404;
//         Serial.println("Sending 404");
//       }
//       server.sendHeader("Access-Control-Allow-Origin", "*");
//       server.send(statusCode, "application/json", content);
//     });
//   }
// }
