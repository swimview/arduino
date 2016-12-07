//XBOXRECV: Kristian Lauszus' test program
//http://blog.tkjelectronics.dk/2012/12/xbox-360-receiver-added-to-the-usb-host-library/
 
#include <XBOXRECV.h>
// Satisfy the IDE, which needs to see the include statement in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
//environment variables
USB Usb;
XBOXRECV Xbox(&Usb);

bool change = false;
int minLvl = 0,
    maxLvl = 240,
    numSteps = 240;
int curLvl = (maxLvl - minLvl) / 2;
    
int increment = (maxLvl - minLvl) / numSteps;
int delayTime = 1000 / numSteps;

int //ouptut pins
    recordOut = 2,
    outputPin = 3,
    statusPin = 4,
    powerPin  = 5,
    killPin   = 6,
    limitPin  = 7;

float percentageOffZeroed = (float) curLvl / (float) maxLvl;
    //input pins
    //upPin     = 6,
    //downPin   = 7,
    //stopPin   = 8,
    //recordIn  = 9;

void setup() {
  Serial.begin(115200);
  TCCR2B = (TCCR2B & 0b11111000) | 0x02;

  pinMode(outputPin, OUTPUT);
  analogWrite(outputPin, curLvl);

  pinMode(recordOut, OUTPUT);
  pinMode(statusPin, OUTPUT);
  pinMode(powerPin, OUTPUT);

  //pinMode(killPin, INPUT_PULLUP);
  //pinMode(limitPin, INPUT_PULLUP);

  digitalWrite(powerPin, HIGH);
  digitalWrite(recordOut, HIGH);

  //force 10 to output mode
  //pinMode(10,OUTPUT);
  //digitalWrite(10,LOW);

  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (Usb.Init() == -1); //halt
  }
}

void loop() {
  Usb.Task();
  int i = 0;
  if (Xbox.XboxReceiverConnected) {
  if (Xbox.Xbox360Connected[i] && digitalRead(killPin) != HIGH) {
    change = false;

    /*if (digitalRead(limitPin) == LOW) {
      curLvl = maxLvl - minLvl - curLvl; //reverse reverse
      change = true;
      Serial.println("LIMIT");
    }*/
    //STOP
    if (Xbox.getButtonClick(A, i)) {
      curLvl = (maxLvl - minLvl) / 2;
      change = true;
      Serial.println("stopBtn");
    } 
    //RECORD
    else if (Xbox.getButtonClick(B, i)) {
      //stop cart while record signal sending
      curLvl = (maxLvl - minLvl) / 2;
      analogWrite(outputPin, curLvl);

      //drop record signal low
      Serial.println("recordBtn");
      digitalWrite(recordOut, LOW);
      delay(1000);
      digitalWrite(recordOut, HIGH);
      
    } else {
      //read controller
      if (Xbox.getAnalogHat(RightHatX, i) > 7500) {
        curLvl += increment;
        change = true;
      } else if (Xbox.getAnalogHat(RightHatX, i) < -7500) {
        curLvl -= increment;
        change = true;
      }
    }
    //filter output to be inbounds before write
    if (curLvl < minLvl) { curLvl = minLvl; }
    if (curLvl > maxLvl) { curLvl = maxLvl; }
  } else {
  //blink
    Serial.println("Not connected!");
    change = !change;
    curLvl = (maxLvl - minLvl) / 2;
    digitalWrite(powerPin, change);
    digitalWrite(statusPin, change);
    delay(1000); 
  }
  } else {
  //blink
    Serial.println("Not connected!");
    change = !change;
    curLvl = (maxLvl - minLvl) / 2;
    digitalWrite(powerPin, change);
    digitalWrite(statusPin, change);
    delay(1000); 
  }

  if (change) {
    digitalWrite(statusPin, HIGH);
    analogWrite(outputPin, curLvl);
    Serial.println(curLvl);
    digitalWrite(statusPin, LOW);
  }
  delay(delayTime);
}
