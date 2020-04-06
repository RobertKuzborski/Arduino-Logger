

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <EEPROM.h>

#include "RTClib.h"

RTC_DS1307 RTC;


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//button States monitoring in BITS
byte buttonnow = 1024; //stores button values and default is high
byte buttonpast = 1024;

byte buttonscoring = 0;
unsigned int howmanytimesbuttonpushed;
//button timing 
unsigned long timethreshold = 2; // in seconds
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
  pinMode(8, INPUT_PULLUP);

  if(!digitalRead(8))EEPROMUpdateInt(2,0); //reset EEPROM counter

   howmanytimesbuttonpushed = EEPROMReadInt(2);
       
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
          //howmanytimesbuttonpushed++; // moved to file writing section // this is continuing growing when pressed, make it happen once (debounce in a way)
          filewriteallowed = 1; // replace that with goto either allow that to be logged twice , because if 2 buttons pressed at the same second will cause only 1 record...
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


  lcd.setCursor(0,1);
  lcd.print(howmanytimesbuttonpushed); 

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

  if(filewriteallowed)  // and make for loop for 4 and relying on buttonnow decide to write mutiple times if buttons were pressed at exactly same time
  {
    for (int i = 0; i<4; i++)
    {
      if(!bitRead(buttonnow,i)) //if currently multiple button is pressed log each of them 
      {
        howmanytimesbuttonpushed++;
  file.print(howmanytimesbuttonpushed);
  file.print(";");
  file.print(i+1);
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
     }
  
  }
  file.close();
  

  
  filewriteallowed = 0;
  buttonpast = buttonnow;
  //number=howmanytimesbuttonpushed;
  EEPROMUpdateInt(2,howmanytimesbuttonpushed);
  if (howmanytimesbuttonpushed > 65000)howmanytimesbuttonpushed=0;
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


//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMUpdateInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.update(p_address, lowByte);
     EEPROM.update(p_address + 1, highByte);
     }

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
     }

//void EEPROMloggingReset(int Eaddress, int Eyear, int Emonth, int Eday, int Ehour, int Eminute)
//{
//  EEPROMUpdateInt( Eaddress, int Eyear);
//  EEPROMUpdateInt( Eaddress+2, int Emonth);
//  EEPROMUpdateInt( Eaddress+4, int Eday);
//  EEPROMUpdateInt( Eaddress+6, int Ehour);
//  EEPROMUpdateInt( Eaddress+8, int Eminute);
//  EEPROMUpdateInt( Eaddress+11, 0); //total of eeprom hours logged
//  for (int i = Eaddress+20, i < Eaddress+20+672; I++) EEPROM.update(i, 0); //clear log data
//  
//  
//}
//
//void EEPROMloggingExecute(int Eaddress, int EEloghour, int Eamount)
//{
//  //int shifthour EEPROMReadInt(int p_address)
//  EEPROMUpdateInt( Eaddress+20+EEloghour, int Eamount);
//}


//counter
//
//log every hour to new eeprom address starting from 10
//
//do a backup for 1 week.
//
//1024 bytes in total
//
//so need to express 1 week in hours
//
//7 days x 24 hours = 168 hours 
//
//168 hours x 2 bytes = 336 bytes
//
//let's start from 100 byte offset + 336 bytes = 436.
//
//lets take 2 weeks of time then total size we need is 672 bytes.
//
//so first day and hour of logging is determined by weekdays. 
//
//address = 100 + weekday*24
//
//so as well store start date in eeprom! 
//
//max buffer 336 hours.
//create hour counter function
//start hour == day
//
//if(last hour != current hour)
//eeprom writeint (eepromhour, numberofusers)
//eepromhour ++
//
//reset:
//start time = current time (int year, int month, int day, int hour, int minute int second)
//eepromhours = 0
//lasthour=currenthour
//wipe out eeprom memory section == 0;

  

