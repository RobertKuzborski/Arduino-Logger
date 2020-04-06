

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


byte buttonnow = 1024; //stores button values and default is high
byte buttonpast = 1024;


byte buttonscoring = 0;
int howmanytimesbuttonpushed;

unsigned long timethreshold = 2;
unsigned long currenttime;
unsigned long pushtime[4] = {0,0,0,0} ;

bool filewriteallowed = 0;

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
   DateTime now = RTC.now(); 
  currenttime = now.unixtime();
  buttonnow = PIND >> 2;

  for(int i=0; i<4; i++)
  {
    
    if (bitRead(buttonnow,i) != bitRead(buttonpast,i) && bitRead(buttonnow,i) == LOW) pushtime[i] = currenttime;
    else if (bitRead(buttonnow,i) == LOW && bitRead(buttonpast,i) == LOW)
    {
      if (currenttime - pushtime[i] >= timethreshold )
      {
        if (!bitRead(buttonscoring,i))
        {
          howmanytimesbuttonpushed++;  // this is continuing growing when pressed, make it happen once (debounce in a way)
          filewriteallowed = 1;
        }
        //log that to sd card 
        bitWrite(buttonscoring,i,HIGH);
      }
    }
    else if (bitRead(buttonnow,i) != bitRead(buttonpast,i) && bitRead(buttonnow,i) == HIGH) 
    {
      //releasetime = currenttime;
      //durationtime = releasetime - pushtime;
      // log that somewhere if needed
      bitWrite(buttonscoring,i,LOW);
     } 
    else if (bitRead(buttonnow,i) == bitRead(buttonpast,i)); //nothing happens a.k.a default button state is HIGH;
    else;
      
  }
  
  
 lcd.clear();
  
 
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
    if(bitRead(buttonnow,i)) lcd.print("-");  // !!!! not pressed button  =  1 because internal pullup
    else if(bitRead(buttonscoring,i)) lcd.print("*");
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
  if (file)  {    lcd.print("SD");   }
  else  {    lcd.print("!!");   }
  delay(50);

  if(filewriteallowed)
  {
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
  
  }
  file.close();
  


  filewriteallowed = 0;
  buttonpast = buttonnow;
  number=howmanytimesbuttonpushed;
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




