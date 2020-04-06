

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "RTClib.h"

RTC_DS1307 RTC;


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

unsigned int number = 0;
char incomingByte;
int lines = 0;
int chars = 0;
//bool occupied[4] = {0,0,1,0};
byte portd = 0; //stores button values
byte lastportd = 0;
byte lastminutes;
byte lastseconds;
byte portdoccupied;
unsigned long time;
unsigned long lasttime[4] = {0,0,0,0} ;

int CS_PIN = 10;
File file;

void setup()   {       
  DDRD = B00000010;    //inputs serial io
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
       
  //Serial.begin(9600);
  
  lcd.begin(16,2);
  lcd.backlight();
  
    Wire.begin();
    RTC.begin();
  // Check to see if the RTC is keeping time.  If it is, load the time from your computer.
  if (! RTC.isrunning()) {
    lcd.clear();
    lcd.print("RTC is NOT running!");
    // This will reflect the time that your sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
   initializeSD();
   delay(2000);

}


void loop() {
  time = millis();
  portd = PIND >> 2;

  for(int i=0; i<4; i++)
  {
    if(bitRead(portd,i) != bitRead(lastportd,i) && !bitRead(portd,i)) lasttime[i] = time;//buton state changed to LOW (on)
    if(bitRead(portd,i) != bitRead(lastportd,i) && bitRead(portd,i)) bitWrite(portdoccupied,i,LOW);//buton state changed to HIGH (off)
    
    else if (bitRead(portd,i) == bitRead(lastportd,i && bitRead(portd,i) == LOW)) //if button state is the same and PRESSED(LOW) monitor millis and wait untill button being released
    {
      if (lasttime[i] + 1000 < time) bitWrite(portdoccupied,i,HIGH);
    }
    else // if button is low 
    {
      lasttime[i] = time;
    }
      
  }
  
  
 lcd.clear();
  
  DateTime now = RTC.now(); 
    lcd.print(now.day(), DEC);
    lcd.print('-');
    lcd.print(now.month(), DEC);
    //lcd.print('-');
    //lcd.print(now.year(), DEC);
    lcd.print("   ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    //lcd.print('    ');    
    
    
  //
  //lcd.println("");

  lcd.setCursor(0,1);
  lcd.print(number); 

  lcd.setCursor(7,1);

//  for(int i=0; i<4; i++)
//  {
//    if(occupied[i] == HIGH) lcd.print("*"); 
//    else lcd.print("-");
//  }

  for(int i=0; i<4; i++)
  {
    if(bitRead(portd,i)) lcd.print("-");  // !!!! not pressed button  =  1 because internal pullup
    else if(bitRead(portdoccupied,i)) lcd.print("*");
    else lcd.print("+");
  }
  

  if(!SD.exists("test.csv"))
  {
    file = SD.open("test.csv", FILE_WRITE);
    file.println("sep=;");
    file.println("PersonNr; PissoirNr; Date ");
    file.close();
  }
 

  file = SD.open("test.csv", FILE_WRITE);

    lcd.setCursor(14,1);  
  if (file)
  {
    lcd.print("SD"); 
  }
  else
  {
    lcd.print("!!");
    
  }
  delay(500);
  
  file.print(number);
  file.print(";");
  file.print(number/2);
  file.print(";");
    file.print(now.day(), DEC);
    file.print('-');
    file.print(now.month(), DEC);
    file.print('-');
    file.print(now.year(), DEC);
    file.print(' ');
    file.print(now.hour(), DEC);
    file.print(':');
    file.print(now.minute(), DEC);
    file.print(':');
    file.print(now.second(), DEC);
    file.println();    
  
  
  file.close();



  
  lastportd = portd;
  number++;
  if (number > 65000)number=0;
}



void initializeSD()
{
 // Serial.println("InitSD");
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin())
  {
  lcd.print("SD ready!");
  } else
  {
  lcd.print("SD init failed!");
    return;
  }
}




