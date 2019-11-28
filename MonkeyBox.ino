#include <DFPlayerMini.h>
#include <Servo.h>


#define DFPLAYER_BUSY_PIN 6
#define DFPLAYER_RX_PIN   7
#define DFPLAYER_TX_PIN   4
#define ARM_SERVO_PIN     5
#define LID_SERVO_PIN     3
#define SWITCH_PIN        8

#define MOOD_COOLDOWN_TIME 30000


enum MonkeyMood {
  moodStarting = -1,
  moodAsleep = 0,
  moodSleepy,
  moodGrumpy,
  moodAnnoyed,
  moodSuspicious,
  moodWarning,
  moodAngry,
  moodFurious
};




Servo armServo;
Servo lidServo;
DFPlayerMini soundPlayer;
int armPos = 0;
MonkeyMood monkeyMood = moodStarting;
unsigned long moodChangeTime;
bool switchOn = false;
bool prevSwitchOn = false;
int armSpeed = 0;

void setup() {
  Serial.begin(9600);

  armServo.attach(ARM_SERVO_PIN);
  lidServo.attach(LID_SERVO_PIN);

  pinMode(SWITCH_PIN, INPUT);
  
  soundPlayer.init(DFPLAYER_BUSY_PIN, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN, soundLoop);
  //soundPlayer.playFile(2);

  monkeyMood = moodAsleep;
  moodChangeTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  soundLoop();
  //soundPlayer.playFile(2);
}


void soundLoop() {
  unsigned long currentTime = millis();
  if(abs(currentTime - moodChangeTime) > MOOD_COOLDOWN_TIME && monkeyMood > moodAsleep) {
    monkeyMood = (int)monkeyMood - 1;
    moodChangeTime = currentTime;
  }
  
  switchOn = (digitalRead(SWITCH_PIN) == HIGH);
  if(prevSwitchOn != switchOn) {
    prevSwitchOn = switchOn;
    if(switchOn) {
      moodChangeTime = currentTime;
      monkeyMood = (int)monkeyMood + 1;
      if(monkeyMood > moodFurious) {
        monkeyMood = moodFurious;
      }
      doSwitchedOn();
    } else {
      doSwitchedOff();
    }
  }

  doTick();
  
  armPos += armSpeed;
  if(armPos > 180) {
    armPos = 180;
  }
  if(armPos < 0) {
    armPos = 0;
  }

  /*Serial.print("Arm Pos: ");
  Serial.print(armPos);
  Serial.print(" State: ");
  Serial.println(monkeyMood);*/

  armServo.write(armPos);
  lidServo.write(90);
  delay(20);
}

void doSwitchedOn() {
  switch(monkeyMood) {
    case moodAsleep:
      armSpeed = 1;
      break;
    case moodSleepy:
      armSpeed = 2;
      break;
    case moodGrumpy:
      armSpeed = 3;
      soundPlayer.playFile(0);
      break;
    case moodAnnoyed:
      armSpeed = 4;
      soundPlayer.playFile(1);
      break;
    case moodSuspicious:
      armSpeed = 5;
      soundPlayer.playFile(2);
      break;
    case moodWarning:
      armSpeed = 10;
      soundPlayer.playFile(3);
      break;
    case moodAngry:
      armSpeed = 20;
      soundPlayer.playFile(4);
      break;
    case moodFurious:
      armSpeed = 20;
      soundPlayer.playFile(5);
      break;
  }
}

void doSwitchedOff() {
  switch(monkeyMood) {
    case moodAsleep:
      armSpeed = -1;
      break;
    case moodSleepy:
      armSpeed = -2;
      break;
    case moodGrumpy:
      armSpeed = -3;
      break;
    case moodAnnoyed:
      armSpeed = -5;
      break;
    case moodSuspicious:
      armSpeed = -20;
      break;
    case moodWarning:
      armSpeed = -5;
      break;
    case moodAngry:
      armSpeed = -20;
      break;
    case moodFurious:
      armSpeed = -10;
      break;
  }
}

void doTick() {
  switch(monkeyMood) {
    case moodAsleep:
      break;
    case moodSleepy:
      break;
    case moodGrumpy:
      break;
    case moodAnnoyed:
      break;
    case moodSuspicious:
      break;
    case moodWarning:
      break;
    case moodAngry:
      break;
    case moodFurious:
      break;
  }
}
