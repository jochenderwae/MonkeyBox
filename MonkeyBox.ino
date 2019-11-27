#include <DFPlayerMini.h>
#include <Servo.h>


#define DFPLAYER_BUSY_PIN 6
#define DFPLAYER_RX_PIN   7
#define DFPLAYER_TX_PIN   4
#define ARM_SERVO_PIN     5
#define LID_SERVO_PIN     3
#define SWITCH_PIN        8


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
bool switchOn = false;

void setup() {
  Serial.begin(9600);

  armServo.attach(ARM_SERVO_PIN);
  lidServo.attach(LID_SERVO_PIN);

  pinMode(SWITCH_PIN, INPUT);
  
  soundPlayer.init(DFPLAYER_BUSY_PIN, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN, soundLoop);
  //soundPlayer.playFile(2);

  monkeyMood = moodAsleep;
}

void loop() {
  // put your main code here, to run repeatedly:
  /*for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }*/
  soundLoop();
}


void soundLoop() {
   switchOn = (digitalRead(SWITCH_PIN) == HIGH);

   switch(monkeyMood) {
      case moodAsleep:
        if(switchOn) {
          monkeyMood = moodSleepy;
        }
        break;
      case moodSleepy:
        if(switchOn) {
          armPos += 10;
          if(armPos > 180) {
            armPos = 180;
          }
        } else {
          armPos -= 10;
          if(armPos <= 0) {
            armPos = 0;
            monkeyMood = moodAsleep;
          }
        }
        break;
   }

   Serial.print("Arm Pos: ");
   Serial.print(armPos);
   Serial.print(" State: ");
   Serial.println(monkeyMood);

   armServo.write(armPos);
   delay(10);
}
