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
#define SERVO_OUTPUT V2
#define LCD V5

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
bool isOpenLid_flag = false;
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
  {
    led_set[0] = val;
    //    Blynk.virtualWrite(LCD, val);
    Serial.println("Button Pressed");
  }
  if (led_timer_on_set[0] == 0)
  {
    led_set[1] = val;
    //    Blynk.virtualWrite(LCD, val);
    Serial.println("Button Pressed");
  }
  else
  {
    update_blynk_status[0] = 1;
    update_blynk_status[1] = 1;
    Blynk.virtualWrite(LCD, "Waiting...");
    Serial.println("Waiting...");
  }
}
BLYNK_WRITE(LCD)
{
  String text = param.asString();
  Blynk.virtualWrite(LCD, "print");
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
  
  for (int i=0; i<2; i++)
  {
    led_status_buf[i] = led_status[i];
    time_set_overflow = 0;
    
    if ( timer_start_set[i] != 0xFFFF && timer_stop_set[i] != 0xFFFF)
    {
      if ( timer_stop_set[i] < timer_start_set[i] ) time_set_overflow = 1;

//      debug
      Serial.print("rtc_sec: ");
      Serial.println(rtc_sec);
      Serial.print("time_stop: ");
      Serial.println(timer_stop_set[i]);
      Serial.print("time_start: ");
      Serial.println(timer_start_set[i]);
      

      if ((((time_set_overflow == 0 && (rtc_sec >= timer_start_set[i]) && (rtc_sec < timer_stop_set[i])) ||
        (time_set_overflow  && ((rtc_sec >= timer_start_set[i]) || (rtc_sec < timer_stop_set[i])))) && 
        (weekday_set[i] == 0x00 || (weekday_set[i] & (0x01 << (day_of_week - 1) )))) )
        {
          led_timer_on_set[i] = 1;
           Serial.print("led_timer_on_set[");
          Serial.print(i);
          Serial.println("] = 1");
        }
        else
          led_timer_on_set[i] = 0;
    }
    else
      led_timer_on_set[i] = 0;

    if ( led_timer_on_set[i] )
    {
      led_status[i] = 1;
      led_set[i] = 0;
    }
    else
    {
      led_status[i] = led_set[i];
    }

    if ( led_status_buf[i] != led_status[i] )
      update_blynk_status[i] = 1;  
  }
    // HARDWARE CONTROL
  // digitalWrite(D0, led_status[0]);  
  digitalWrite(GREEN_LIGHT, led_status[1]);  
}
//------------------------------------//
void blynk_update()
{
  if (update_blynk_status[0])
  {
    update_blynk_status[0] = 0;
    Blynk.virtualWrite(V1, led_status[0]);
  }

  if (update_blynk_status[1])
  {
    update_blynk_status[1] = 0;
    Blynk.virtualWrite(V1, led_status[1]);
  }
}
//------------------------------------------------//
void openLid_flag()
{
  if (led_timer_on_set[1] == 1 || led_timer_on_set[0] == 1)
    isOpenLid_flag = true;
  else if (led_set[0] == 1 || led_set[1] == 1)
    isOpenLid_flag = true;
  else
    isOpenLid_flag = false;
}

void openLid()
{
  if (isOpenLid_flag == true && idle_position != servo_on)
  {
    digitalWrite(GREEN_LIGHT, HIGH);
    lcd.clear();
    text = "Feeding";
    lcd.setCursor(0, 0);
    lcd.print("Feeding");
    // Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(13);
    servo.write(servo_on);
    Blynk.virtualWrite(V2, servo_on);
    Blynk.virtualWrite(LCD, text);
    delay(500);
    servo.detach();
    delay(500);
    digitalWrite(BUZZER, LOW);
    idle_position = servo_on;
    //    Serial.print(" ", + idle_position);
  }
  if (isOpenLid_flag == false && idle_position != servo_off)
  {
    digitalWrite(GREEN_LIGHT, LOW);
    lcd.clear();
    text = "Stop";
    lcd.setCursor(0, 0);
    lcd.print("Stop");
    // Serial.print(text);
    digitalWrite(BUZZER, HIGH);
    servo.attach(13);
    servo.write(servo_off);
    Blynk.virtualWrite(V2, servo_off);
    Blynk.virtualWrite(LCD, text);
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
  openLid_flag();
  openLid();
}
