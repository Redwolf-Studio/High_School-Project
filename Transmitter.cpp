/*
LoRa Node transmitter
Ver. 1.1 (TEST)
----------------------------------- info ----------------------------------
IDE software          : Arduino IDE or Atom IDE with PlatformIO
hardware controller   : Arduino MEGA 2560
module and sensor     : -LoRa
                        -OLED display (ISP)
                        -Volt sensor 
                        -Current sensor
---------------------------------------------------------------------------
Developer : 1.Worakan Chantima (Arduino Code)
            2.Siravit Wiangkham (Visual Studio Code)
Advisoes : Kridsada Luangthongkham
Demonstration School University of Phayao
- DeSUP -
*/

#include <Arduino.h>
#include <avr/wdt.h> //Watchdog function
#include <SPI.h>
#include <Wire.h>


#include <LoRa.h>
#include <RTClib.h>


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>     //Color OLED SSD1331 SPI
#include <Adafruit_INA219.h>      //Digital Current Sensor (GY-INA219)

int disp = 6;
int updateDISP = 7;
int OLED_Scene = 0;
//volt sensor
float R1 = 27000;
float R2 = 15000;

boolean BATTService;

DS3231 RTC;

//Up to 4 boards INA219 module
Adafruit_INA219 CerSensor1(0x40); //SOLA
Adafruit_INA219 CerSensor2(0x41); //BATTERY
Adafruit_INA219 CerSensor3(0x44); //OUT
//Adafruit_INA219 CerSensor3(0x45);

/*
***************************************************************************
*                             OLED Setting                                *
***************************************************************************
*/
//Color OLED SSD1331 SPI

//#define sclk 3
//#define mosi 4
#define cs   11
#define rst  12
#define dc   13

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, rst);

const unsigned char Battery25[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0,
	0x04, 0x20, 0x3c, 0x3c, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04,
	0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x2f, 0xf4,
	0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery50[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0,
	0x04, 0x20, 0x3c, 0x3c, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04,
	0x20, 0x04, 0x20, 0x04, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4,
	0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery75[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0,
	0x04, 0x20, 0x3c, 0x3c, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4,
	0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4,
	0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery100[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0,
	0x04, 0x20, 0x3d, 0xbc, 0x21, 0x84, 0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4,
	0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x2f, 0xf4,
	0x2f, 0xf4, 0x2f, 0xf4, 0x2f, 0xf4, 0x20, 0x04, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char BatteryLOW[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0,
	0x04, 0x20, 0x3c, 0x3c, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x21, 0x84, 0x23, 0xc4, 0x23, 0xc4,
	0x23, 0xc4, 0x23, 0xc4, 0x23, 0xc4, 0x23, 0xc4, 0x21, 0x84, 0x21, 0x84, 0x20, 0x04, 0x21, 0x84,
	0x21, 0x84, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char SolaICON[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf8,
	0x3c, 0x01, 0x80, 0x3c, 0x39, 0xbd, 0xbd, 0x9c, 0x33, 0xbd, 0xbd, 0xcc, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x30, 0x01, 0x80, 0x0c, 0x37, 0xbd, 0xbd, 0xec, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x37, 0xb9, 0x9d, 0xec, 0x30, 0x03, 0xc0, 0x0c, 0x3f, 0xfe, 0x7f, 0xfc,
	0x3f, 0xfe, 0x7f, 0xfc, 0x30, 0x03, 0xc0, 0x0c, 0x37, 0xb9, 0x9d, 0xec, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x37, 0xbd, 0xbd, 0xec, 0x30, 0x01, 0x80, 0x0c, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x33, 0xbd, 0xbd, 0xcc, 0x39, 0xbd, 0xbd, 0x9c, 0x3c, 0x01, 0x80, 0x3c,
	0x1f, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char SolaGenICON[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf8,
	0x3c, 0x01, 0x80, 0x3c, 0x39, 0xbd, 0xbd, 0x9c, 0x33, 0xbd, 0xbd, 0xcc, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x30, 0x01, 0x80, 0x0c, 0x37, 0xbd, 0xbd, 0xec, 0x37, 0xbd, 0xbd, 0xec,
	0x37, 0xbd, 0xbd, 0xec, 0x37, 0xb9, 0x9d, 0xec, 0x30, 0x03, 0xc0, 0x0c, 0x3f, 0xfe, 0x78, 0x00,
	0x3f, 0xfe, 0x73, 0xfc, 0x30, 0x03, 0xc4, 0x02, 0x37, 0xb9, 0x8b, 0xfa, 0x37, 0xbd, 0x8b, 0xf4,
	0x37, 0xbd, 0x97, 0xec, 0x37, 0xbd, 0x97, 0xe2, 0x30, 0x01, 0xaf, 0xf4, 0x37, 0xbd, 0xa1, 0xe8,
	0x37, 0xbd, 0x9d, 0xd0, 0x33, 0xbd, 0x85, 0xa0, 0x39, 0xbd, 0x8b, 0x40, 0x3c, 0x01, 0x8a, 0x80,
	0x1f, 0xff, 0xd5, 0x00, 0x0f, 0xff, 0xd2, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char OutputICON[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  	0x00, 0x28, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0xba, 0x00, 0x00, 0x01, 0x7d, 0x00, 0x00,
  	0x02, 0xfe, 0x80, 0x00, 0x05, 0xff, 0x40, 0x00, 0x0b, 0xff, 0xa0, 0x00, 0x08, 0x7c, 0x20, 0x00,
  	0x07, 0x7d, 0xc0, 0x00, 0x01, 0x7d, 0x00, 0x00, 0x01, 0x7d, 0x00, 0x00, 0x01, 0x7d, 0x0f, 0xf0,
	0x01, 0x7d, 0x10, 0x08, 0x01, 0x7d, 0x2f, 0xe8, 0x01, 0x7d, 0x2f, 0xd0, 0x01, 0x7d, 0x5f, 0xb0,
  	0x01, 0x7d, 0x5f, 0x88, 0x01, 0x7d, 0xbf, 0xd0, 0x01, 0x7d, 0x87, 0xa0, 0x01, 0x7d, 0x77, 0x40,
	0x01, 0x7d, 0x16, 0x80, 0x01, 0x7d, 0x2d, 0x00, 0x01, 0x7d, 0x2a, 0x00, 0x01, 0x01, 0x54, 0x00,
  	0x01, 0xff, 0x48, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char WindICON[] PROGMEM = {
  	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x02, 0x40, 0x00,
	0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00,
	0x00, 0x24, 0x48, 0x00, 0x00, 0x64, 0x46, 0x00, 0x00, 0x84, 0x41, 0x00, 0x01, 0x05, 0x58, 0x80,
	0x02, 0x29, 0x44, 0x40, 0x06, 0x49, 0x42, 0x40, 0x04, 0x89, 0x41, 0x20, 0x05, 0x04, 0x41, 0x20,
	0x09, 0x07, 0xc1, 0xb0, 0x09, 0x06, 0x60, 0x90, 0x09, 0x0c, 0x3c, 0x90, 0x08, 0x1c, 0x23, 0x10,
	0x08, 0x66, 0x19, 0x80, 0x00, 0x8f, 0x06, 0x40, 0x03, 0x19, 0x73, 0x20, 0x04, 0x62, 0x0c, 0x10,
	0x18, 0x0c, 0x06, 0x08, 0x20, 0xf0, 0x01, 0x84, 0x47, 0x84, 0x30, 0x62, 0x38, 0x03, 0xc2, 0x1a,
	0x00, 0x60, 0x04, 0x04, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00
};



/*
***************************************************************************
*                             LoRa Setting                                *
***************************************************************************
*/
#define NSS_PIN    10
#define NRESET_PIN 9
#define DIO0_PIN   8

/*
***************************************************************************
*                           OLED Sceen                                    *
***************************************************************************
*/
void SceneBATTERY(float dataV,float dataA,float dataW,unsigned int BattLife) {
  display.fillScreen(BLACK);
  display.fillRect(0, 0, 128, 16, WHITE);
  display.setCursor(1,1);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("BATTERY");

  display.setTextSize(1);
  display.setCursor(38,20);
  display.setTextColor(YELLOW);
  display.print("V: ");
  display.setTextColor(WHITE);
  display.print(dataV);
  display.setCursor(38,30);
  display.setTextColor(YELLOW);
  display.print("A: ");
  display.setTextColor(WHITE);
  display.print(dataA); // now no A sensor
  display.setCursor(38,40);
  display.setTextColor(YELLOW);
  display.print("W: ");
  display.setTextColor(WHITE);
  display.print(dataW); //now no A sensor
  display.setCursor(38,50);
  display.setTextColor(YELLOW);
  display.print(BattLife);
  display.print("%");

  if(BattLife <= 100) {
    display.drawBitmap(10, 20, Battery100, 16, 32, GREEN, BLACK);
  }

  if(BattLife <= 75) {
    display.drawBitmap(10, 20, Battery75, 16, 32, GREEN, BLACK);
  }

  if(BattLife <= 50) {
    display.drawBitmap(10, 20, Battery50, 16, 32, YELLOW, BLACK);
  }

  if(BattLife <= 25) {
    display.drawBitmap(10, 20, Battery25, 16, 32, RED, BLACK);
  }

  if(BattLife < 10) {
    display.drawBitmap(10, 20, BatteryLOW, 16, 32, RED, BLACK);
  }

}//sceen1

void SceneSOLA(float dataV,float dataA,float dataW) {
  display.fillScreen(BLACK);
  display.fillRect(0, 0, 128, 16, WHITE);
  display.setCursor(1,1);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("SOLA");

  display.setTextSize(1);
  display.setCursor(38,20);
  display.setTextColor(YELLOW);
  display.print("V: ");
  display.setTextColor(WHITE);
  display.print(dataV);
  display.setCursor(38,30);
  display.setTextColor(YELLOW);
  display.print("A: ");
  display.setTextColor(WHITE);
  display.print(dataA);
  display.setCursor(38,40);
  display.setTextColor(YELLOW);
  display.print("W: ");
  display.setTextColor(WHITE);
  display.print(dataW);

  display.drawBitmap(3, 20, SolaICON, 32, 32, WHITE, BLACK);

  if(dataV > 0) {
    display.drawBitmap(3, 20, SolaGenICON, 32, 32, 0xFAE0, BLACK);
  }

  if(dataV > 11.5) {
    display.drawBitmap(3, 20, SolaGenICON, 32, 32, 0x051F, BLACK);
  }


}//sceen2

void SceneOUTPUT(float dataV,float dataA,float dataW) {
  display.fillScreen(BLACK);
  display.fillRect(0, 0, 128, 16, WHITE);
  display.setCursor(1,1);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("OUTPUT");

  display.setTextSize(1);
  display.setCursor(38,20);
  display.setTextColor(YELLOW);
  display.print("V: ");
  display.setTextColor(WHITE);
  display.print(dataV);
  display.setCursor(38,30);
  display.setTextColor(YELLOW);
  display.print("A: ");
  display.setTextColor(WHITE);
  display.print(dataA);
  display.setCursor(38,40);
  display.setTextColor(YELLOW);
  display.print("W: ");
  display.setTextColor(WHITE);
  display.print(dataW);

  display.drawBitmap(3, 20, OutputICON, 32, 32, WHITE, BLACK);
}//sceen3

void SceneWindSpeed(float VoltWind, float SpeedWind) {
  display.fillScreen(BLACK);
  display.fillRect(0, 0, 128, 16, GREEN);
  display.setCursor(1,1);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.print("WIND");

  display.setTextSize(1);
  display.setCursor(38,20);
  display.setTextColor(YELLOW);
  display.print("Volt:");
  display.setTextColor(WHITE);
  display.print(VoltWind);

  display.setCursor(38,30);
  display.setTextColor(CYAN);
  display.println("Speed m/s");
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(38,40);
  display.print(SpeedWind);

  display.drawBitmap(3, 20, WindICON, 32, 32, WHITE, BLACK);
}//Show wind speed

void SaveDisplay() {
  display.fillScreen(BLACK);
}//SAVE Display

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

/*
***************************************************************************
*                           void setup                                    *
***************************************************************************
*/
void setup() {
  Serial.begin(9600);   //Serial
  Wire.begin();         //i2c
  RTC.begin();          //RTC
  wdt_enable(WDTO_8S);  //(Watchdog) if notrespond longer 8sec.
  CerSensor1.begin();   //INA219 Senser 1
  CerSensor2.begin();   //INA219 Senser 2
  CerSensor3.begin();   //INA219 Senser 3

  tone(53, 1000); //sound notification
  delay(100);
  noTone(53);
  delay(50);
//LoRa
  while(!Serial);
  Serial.println("LoRa Node - Transmitter");

  LoRa.setTxPower(1); //1-17 dBm
  LoRa.setPins(NSS_PIN, NRESET_PIN, DIO0_PIN);
  LoRa.begin(433E6);

  if(!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while(1);
  }
  LoRa.setTimeout(100);

//RTC
  RTC.adjust(DateTime(__DATE__, __TIME__));

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

//Color OLED SSD1331 SPI
  display.begin();
  display.fillScreen(BLACK);

  OLED_Scene = 1;

//Pin mode
  pinMode(disp, INPUT_PULLUP);
  pinMode(updateDISP, INPUT_PULLUP);

//Value setting
  OLED_Scene = 1;
}//void setup

/*
***************************************************************************
*                            void loop                                    *
***************************************************************************
*/
void loop() {

//timing control
  static unsigned long Time = millis();
  DateTime now = RTC.now();

  //static unsigned long TestReadValueTime = millis();  //for test read valueI/O (DEBUG)
//Sensor Read
  int ValueV_Sola   = analogRead(2);
  int ValueV_Batt   = analogRead(1);
  int ValueV_out    = analogRead(7);
  int WindValue     = analogRead(8);

  float ValueA_Sola   = CerSensor1.getCurrent_mA();
  float ValueA_Batt   = CerSensor2.getCurrent_mA();
  float ValueA_out    = CerSensor3.getCurrent_mA();
//Calculate data
  //sola
  float Vsola = (ValueV_Sola * 5.00) / 1024.00;
  float V_Sola = (Vsola / (R2/(R1+R2))) - 0.21;
  if(V_Sola < 0.09) {
    V_Sola = 0;
  }

  float A_Sola = ValueA_Sola/1000; //mA to A
  float W_Sola = V_Sola*A_Sola;

  //battery
  float Vbatt = (ValueV_Batt * 5.00) / 1024.00;
  float V_Batt = (Vbatt / (R2/(R1+R2))) - 1.38;
  if(V_Batt < 0.09) {
    V_Batt = 0;
  }

  float A_Batt = ValueA_Batt/1000; //mA to A
  float W_Batt = V_Batt*A_Batt;

  //output
  float Vout = (ValueV_out * 5.00) / 1024.00;
  float V_out = (Vout / (R2/(R1+R2))) - 1.84;
  if(V_Sola < 0.09) {
    V_Sola = 0;
  }

  float A_out = ValueA_out/1000; //mA to A
  float W_out = V_out*A_out;

//Battery Calculate
  float LowBattery = 11.5;
  float FullBattery = 12.5;

  int batteryLife = mapFloat(V_Batt, LowBattery, FullBattery, 0, 100);
  if(V_Batt < LowBattery) {
    batteryLife = 0;
  }
  if(V_Batt >= FullBattery) {
    batteryLife = 100;
  }

  //Windspeed
  double WindVolt = WindValue * (5.00/ 1023.00);
  double WindSpeed = WindVolt * 2.2628;


//button select scene
  while(digitalRead(disp) == HIGH) {
    OLED_Scene++;
    tone(53, 1000); //sound notification
    delay(100);
    noTone(53);

    delay(400);
    Serial.println(OLED_Scene);

    //updateDISP
    if(OLED_Scene == 1) {
      SceneBATTERY(V_Batt, A_Batt, W_Batt, batteryLife);
    }//update scene 1

    if(OLED_Scene == 2) {
      SceneSOLA(V_Sola, A_Sola, W_Sola);
    }//update scene 2

    if(OLED_Scene == 3) {
      SceneOUTPUT(V_out, A_out, W_out);
    }//update scene 3

    if(OLED_Scene == 4) {
      SceneWindSpeed(WindVolt, WindSpeed);
    }//update scene 3

    if(OLED_Scene == 5) { //save screen
      OLED_Scene = 0;
      Serial.println("Save display mode");
    }
  }

//update display
    while(digitalRead(updateDISP) == HIGH) {
      if(OLED_Scene == 1) {
        SceneBATTERY(V_Batt, A_Batt, W_Batt, batteryLife);
      }//update scene 1

      if(OLED_Scene == 2) {
        SceneSOLA(V_Sola, A_Sola, W_Sola);
      }//update scene 2

      if(OLED_Scene == 3) {
        SceneOUTPUT(V_out, A_out, W_out);
      }//update scene 3

      if(OLED_Scene == 4) {
        SceneWindSpeed(WindVolt, WindSpeed);
      }//update scene 4
    }


//OLED Show...
  if((millis()-Time)>2000) { //2sec.
    if(OLED_Scene == 1) { //Battery info
      Serial.println("Scene 1 update!");
      SceneBATTERY(V_Batt, A_Batt, W_Batt, batteryLife);
    }

    if(OLED_Scene == 2) { //Sola info
      Serial.println("Scene 2 update!");
      SceneSOLA(V_Sola, A_Sola, W_Sola);
    }

    if(OLED_Scene == 3) { //Output info
      Serial.println("Scene 3 update!");
      SceneOUTPUT(V_out, A_out, W_out);
    }

    if(OLED_Scene == 4) { //Output info
      Serial.println("Scene 4 update!");
      SceneWindSpeed(WindVolt, WindSpeed);
    }

    Time = millis();
  }

  if(OLED_Scene == 5) { //save screen
    OLED_Scene = 0;
    Serial.println("Save display mode");
  }

  if(OLED_Scene == 0) {
    SaveDisplay();
  }

//Test read valueI/O (DEBUG)
/*
    Serial.println("--------------------------------------------------------");
    Serial.print("RAW V-Sola\t");
    Serial.println(ValueV_Sola);
    Serial.print("RAW A-Sola\t");
    Serial.println(ValueA_Sola);
    Serial.print("RAW V-Batt\t");
    Serial.println(ValueV_Batt);
    Serial.print("RAW A-Batt\t");
    Serial.println(ValueA_Batt);
    Serial.print("RAW V-out\t");
    Serial.println(ValueV_out);
    Serial.print("RAW A-out\t");
    Serial.println(ValueA_out);
    Serial.println("");
    Serial.print("Sola V\t");
    Serial.println(V_Sola);
    Serial.print("Sola A\t");
    Serial.println(A_Sola);
    Serial.print("Sola W\t");
    Serial.println(W_Sola);
    Serial.println("");
    Serial.print("Batt V\t");
    Serial.println(V_Batt);
    Serial.print("Batt A\t");
    Serial.println(A_Batt);
    Serial.print("Batt W\t");
    Serial.println(W_Batt);
    Serial.println("");
    Serial.print("out V\t");
    Serial.println(V_out);
    Serial.print("out A\t");
    Serial.println(A_out);
    Serial.print("out W\t");
    Serial.println(W_out);
    Serial.println("--------------------------------------------------------");
*/

//LoRa Tx data
  if((now.second() % 30) == 0) { //every 30sec.
    /*
    Data block
    V-Sola,A-Sola,W-Sola,V-Batt,A-Batt,W-Batt,V-out,A-out,W-out,BattService....
    */

    Serial.println("LoRa send data succeed!");
    tone(53, 1500); //sound notification
    delay(100);
    noTone(53);
    delay(50);
    tone(53, 1000);
    delay(100);
    noTone(53);
    delay(50);

    LoRa.beginPacket();
    //Add data to Tx
    LoRa.print(V_Sola); LoRa.print(",");
    LoRa.print(A_Sola); LoRa.print(",");
    LoRa.print(W_Sola); LoRa.print(",");
    LoRa.print(V_Batt); LoRa.print(",");
    LoRa.print(A_Batt); LoRa.print(",");
    LoRa.print(W_Batt); LoRa.print(",");
    LoRa.print(V_out); LoRa.print(",");
    LoRa.print(A_out); LoRa.print(",");
    LoRa.print(W_out); LoRa.print(",");
    LoRa.print(batteryLife); LoRa.print(",");
    LoRa.print(BATTService); LoRa.print(",");
    LoRa.print(WindSpeed); //LoRa.print(",");


    LoRa.endPacket();
    //END Tx data
    delay(600);
  }
  wdt_reset(); //(Watchdog) Reset Arduino
}//void loop
