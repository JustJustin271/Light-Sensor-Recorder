#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int powerPin = 2;
const int dataPin = 3;

const int statePin = 13;

//False...False...False!!!
bool systemActive = false;
bool lastPowerState = false;
bool lastDataState = false;
bool change = false;
bool tickUpdate = false;
bool dataUpdate = false;

unsigned long lastTickState = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  //Makes the pin start off low
  pinMode(powerPin, INPUT_PULLUP);
  pinMode(dataPin, INPUT_PULLUP);

  pinMode(statePin, OUTPUT);      
  digitalWrite(statePin, HIGH); 

  Serial.println(F("Light Logger Ready!"));

  lcd.setCursor(0,0);
  lcd.print("Ready!");
  
}

void loop() {

  //States of power and data
  bool powerState = digitalRead(powerPin);
  bool dataState = digitalRead(dataPin);

  //Do we update the data pin?
  if(dataState == LOW && lastDataState == HIGH) {
    dataUpdate = true;
  }  

  lastDataState = dataState;
  
  unsigned long currentTime = millis(); //Records time in ms

  if (powerState == LOW && lastPowerState == HIGH) { systemActive = !systemActive; lcd.clear();
    //Flips the state to save that state now
    //Will turn off while on to not intefer with light levels
    digitalWrite(statePin, systemActive ? LOW : HIGH); 
                                                     
    if(systemActive) { 
      //Turns on
      lcd.backlight(); 
      lcd.display(); 
      change = true;  //Turns on
      lcd.setCursor(0,0);
      lcd.print(F("System Ready!"));
      delay(200);
      
    } else { 
      
      lcd.noBacklight(); 
      lcd.noDisplay(); 
    } //And now turns off

    delay(100);
  }
  
  lastPowerState = powerState;
  
  if(systemActive) {
    //Activates recording
    if((tickUpdate == true) || dataUpdate) {
        digitalWrite(statePin, LOW); 
        delay(50);
      
        int lightRead = analogRead(A0);
  
        int lightLevel = map(lightRead, 150, 950, 0, 100);
        //Sets the values from 150 to 950 to a scale of 0 to 100

        //Restricts the numbers from 0-149 & 951-1023
        if(lightRead < 150) {
          lightLevel = 0;
        }

        if(lightRead > 950) {
          lightLevel = 100;
        }

      //Serial display
        Serial.print(F("Time(sec): "));
        Serial.print(currentTime/60000);
        Serial.print(F("m "));
        Serial.print((currentTime / 1000) % 60);
        Serial.print(F("s "));
        Serial.print(F(" | Light: "));
        Serial.print(lightLevel);
        Serial.println(F("%       "));

      //LCD display
        lcd.setCursor(0,0);
        lcd.print(F("Time: "));
        lcd.print(currentTime/60000);
        lcd.print(F("m "));
        lcd.print((currentTime / 1000) % 60); 
        lcd.print(F("s   ")); 

        lcd.setCursor(0,1);
        lcd.print(F("Light | "));
        lcd.print(lightLevel);
        lcd.print(F(" %           "));

        tickUpdate = false;
        dataUpdate = false;
    }
  }

  //Tick system
    unsigned long currentTickState = (currentTime / 60000);
  
      if (currentTickState != lastTickState) {
      tickUpdate = true; 
      lastTickState = currentTickState;
    }
  }

//Arduino Light Sensor
//February 21st, 2026 :D
