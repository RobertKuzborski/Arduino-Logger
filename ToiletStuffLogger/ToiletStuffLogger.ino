
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

unsigned int number = 0;
char incomingByte;
int lines = 0;
int chars = 0;

int CS_PIN = 10;
File file;

void setup()   {                
  Serial.begin(9600);

  
    Wire.begin();
    RTC.begin();
  // Check to see if the RTC is keeping time.  If it is, load the time from your computer.
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // This will reflect the time that your sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  //delay(1000);

  // Clear the buffer.
  display.clearDisplay();

  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

   initializeSD();

}


void loop() {


//// draw the first ~12 characters in the font
//  testdrawchar();
//  display.display();
//  delay(2000);
//  display.clearDisplay();


  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  DateTime now = RTC.now(); 
    display.print(now.day(), DEC);
    display.print('-');
    display.print(now.month(), DEC);
    display.print('-');
    display.print(now.year(), DEC);
    display.print(' ');
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();    
    //delay(1000);
    
  //
  display.println("");
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.println(number); //display.println(0xDEADBEEF, HEX);
  
  display.setTextSize(2);
  display.println(" _ + X * 0 ");
  
  display.display();
  delay(100);
  display.clearDisplay();

  file = SD.open("test.csv", FILE_WRITE);
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



  

  number++;
  if (number > 99999)number=0;
}



void initializeSD()
{
  Serial.println("Initializing SD card...");
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
}

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
  delay(1);
}


