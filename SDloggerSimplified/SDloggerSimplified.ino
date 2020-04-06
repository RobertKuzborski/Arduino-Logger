#include <SD.h>
#include <SPI.h>

int CS_PIN = 10;

// button stuff start
const int  buttonPin = 3;    // the pin that the pushbutton is attached to
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
// button stuff end

File file;

void setup()
{

  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  initializeSD();

}


void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState && lastButtonState != HIGH) {
    // if the state has changed, increment the counter
    
      file = SD.open("test.csv", FILE_WRITE);
      file.println("1;3;21/05/2014");//timelog goes here
      closeFile();
      
      Serial.println("on");

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState = buttonState;
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

int createFile(char filename[])
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    Serial.println("File created successfully.");
    return 1;
  } else
  {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(char text[])
{
  if (file)
  {
    file.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else
  {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile()
{
  if (file)
  {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  file = SD.open(filename);
  if (file)
  {
    Serial.println("File opened with success!");
    return 1;
  } else
  {
    Serial.println("Error opening file...");
    return 0;
  }
}

String readLine()
{
  String received = "";
  char ch;
  while (file.available())
  {
    ch = file.read();
    if (ch == '\n')
    {
      return String(received);
    }
    else
    {
      received += ch;
    }
  }
  return "";
}

void hasPeed() {
  createFile("test.csv");
  writeToFile("1;3;21/05/2014");
  closeFile();
}






