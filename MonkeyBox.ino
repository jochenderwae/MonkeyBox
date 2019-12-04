#include <DFPlayerMini.h>
#include <Servo.h>


#define DFPLAYER_BUSY_PIN 6
#define DFPLAYER_RX_PIN   7
#define DFPLAYER_TX_PIN   4
#define ARM_SERVO_PIN     5
#define LID_SERVO_PIN     3
#define DOOR_SERVO_PIN    9
#define SWITCH_PIN        8

#define DOOR_SERVO_MIN    0
#define DOOR_SERVO_MAX    180
#define LID_SERVO_MIN     0
#define LID_SERVO_MID1    60
#define LID_SERVO_MID2    120
#define LID_SERVO_MAX     180
#define ARM_SERVO_MIN     0
#define ARM_SERVO_MAX     180

#define MOOD_COOLDOWN_TIME 30000

#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
#define second(x) (x * 1000)
#define minute(x) (x * 60000)

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
Servo doorServo;
DFPlayerMini soundPlayer;
int armPos = 0;
int lidPos = 0;
int armSpeed = 0;
int lidSpeed = 0;
int lidTarget = 0;

MonkeyMood monkeyMood = moodStarting;
int subState = 0;
unsigned long subTime;
unsigned long moodChangeTime;
bool switchOn = false;
bool prevSwitchOn = false;

void setup() {
  Serial.begin(9600);

  armServo.attach(ARM_SERVO_PIN);
  lidServo.attach(LID_SERVO_PIN);
  doorServo.attach(DOOR_SERVO_PIN);

  armServo.write(armPos);
  lidServo.write(armPos);
  doorServo.write(DOOR_SERVO_MIN);

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
      subState = 0;
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
  if(armPos > ARM_SERVO_MAX) {
    armPos = ARM_SERVO_MAX;
  }
  if(armPos < ARM_SERVO_MIN) {
    armPos = ARM_SERVO_MIN;
  }

  int prevLidSign = sgn(lidTarget - lidPos);
  lidPos += lidSpeed;
  if(lidPos > ARM_SERVO_MAX) {
    lidPos = ARM_SERVO_MAX;
  }
  if(lidPos < ARM_SERVO_MIN) {
    lidPos = ARM_SERVO_MIN;
  }
  int lidSign = sgn(lidTarget - lidPos);
  if(lidSign != prevLidSign) {
    lidPos = lidTarget;
    lidSpeed = 0;
  }

  /*Serial.print("Arm Pos: ");
  Serial.print(armPos);
  Serial.print(" Lid Pos: ");
  Serial.print(lidPos);
  Serial.print(" Lid Target: ");
  Serial.print(lidTarget);
  Serial.print(" State: ");
  Serial.print(monkeyMood);
  Serial.println();*/
  
  armServo.write(armPos);
  lidServo.write(lidPos);
  delay(20);
}

void doSwitchedOn() {
  doorServo.write(DOOR_SERVO_MAX);
  switch(monkeyMood) {
    case moodAsleep:
      armSpeed = 1;
      break;
    case moodSleepy:
      armSpeed = 2;
      soundPlayer.playFile(0);
      break;
    case moodGrumpy:
      armSpeed = 3;
      soundPlayer.playFile(1);
      break;
    case moodAnnoyed:
      armSpeed = 4;
      soundPlayer.playFile(2);
      break;
    case moodSuspicious:
      armSpeed = 5;
      soundPlayer.playFile(3);
      break;
    case moodWarning:
      armSpeed = 10;
      soundPlayer.playFile(4);
      break;
    case moodAngry:
      armSpeed = 20;
      soundPlayer.playFile(5);
      break;
    case moodFurious:
      armSpeed = 20;
      soundPlayer.playFile(5);
      break;
  }
}

void doSwitchedOff() {
  doorServo.write(DOOR_SERVO_MIN);
  switch(monkeyMood) {
    case moodAsleep:
      // normally this state isn't reachable
      armSpeed = -1;
      break;
    case moodSleepy:
      armSpeed = -1;
      break;
    case moodGrumpy:
      armSpeed = -2;
      break;
    case moodAnnoyed:
      armSpeed = -3;
      break;
    case moodSuspicious:
      armSpeed = -5;
      break;
    case moodWarning:
      armSpeed = -20;
      break;
    case moodAngry:
      armSpeed = -5;
      break;
    case moodFurious:
      armSpeed = -20;
      break;
  }
}

void doTick() {
  unsigned long timer = millis() - subTime;
  switch(monkeyMood) {
    case moodAsleep:
      break;
    case moodSleepy:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
    case moodGrumpy:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MID1;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
    case moodAnnoyed:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MID2;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(5)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
    case moodSuspicious:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MAX;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
    case moodWarning:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MID2;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          subTime = millis();
          break;
          
        case 3:
          if(timer > second(3)) {
            subState++;
          }
          break;
        case 4:
          lidTarget = LID_SERVO_MID2;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 5:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 6:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          subTime = millis();
          break;
          
        default:
          break;
      }
      break;
    case moodAngry:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MID1;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
    case moodFurious:
      switch(subState) {
        case 0:
          lidTarget = LID_SERVO_MID1;
          lidSpeed = 5;
          subState++;
          subTime = millis();
          break;
          
        case 1:
          if(timer > second(3)) {
            subState++;
          }
          break;

        case 2:
          lidTarget = LID_SERVO_MIN;
          lidSpeed = -5;
          subState++;
          break;
          
        default:
          break;
      }
      break;
  }
}
