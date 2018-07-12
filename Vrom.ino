#include <Arduboy2.h>
float vehicleX = 64;
const byte vehicleY = 54;
float velocity;
byte gear;
short rpm;
int8_t roadCurve;
int8_t roadPitch;
Arduboy2 arduboy;

void track() {
  if (velocity > 0) {
    if (arduboy.everyXFrames(10)) {
      roadCurve = roadCurve + random(-2, 2);
    }
    if (roadCurve > 4) roadCurve = 4;
    if (roadCurve < -4) roadCurve = -4;
    if (arduboy.everyXFrames(max(210 - velocity, 10))) {
      roadPitch = roadPitch + random(-1, 2);
      
      );
    }
    if (roadPitch > 6) roadPitch = 6;
    if (roadPitch < -6) roadPitch = -6;
    vehicleX = vehicleX + roadCurve * 0.25 * velocity / 100;
  }
  for (byte i = 0; i < 48 + 1.5 * roadPitch; i++) {
    arduboy.drawFastHLine((i + 0.4 * i * roadCurve) / 2, 64 - i, 128 - i, 1);
  }
}

void setup() {
  // put your setup code here, to run once
  arduboy.begin();
  arduboy.setFrameRate(30);
  roadCurve = roadPitch = velocity = 0;
  /*
    float loading = 55;
    while (loading > 10) {
    if (arduboy.nextFrame()) {
      arduboy.clear();
      if (arduboy.pressed(A_BUTTON) && loading <= 59) loading = loading - 0.7;
      else if (loading < 60) loading = loading + 0.3;
      if (loading >= 59) loading = 59;
      arduboy.setCursor(35, 55);
      arduboy.print("Loading...");
      arduboy.fillRect(110, 10, 10, 10, 1);
      arduboy.drawRect(110, 20, 10, 40, 1);
      arduboy.drawLine(97, 10, 119, 10, 1);
      arduboy.setCursor(91, 7);
      arduboy.print(100);
      arduboy.drawLine(97, 20, 119, 20, 1);
      arduboy.setCursor(95, 17);
      arduboy.print(80);
      arduboy.drawLine(97, 30, 119, 30, 1);
      arduboy.setCursor(95, 27);
      arduboy.print(60);
      arduboy.drawLine(97, 40, 119, 40, 1);
      arduboy.setCursor(95, 37);
      arduboy.print(40);
      arduboy.drawLine(97, 50, 119, 50, 1);
      arduboy.setCursor(95, 47);
      arduboy.print(20);
      arduboy.drawLine(107, 59, 119, 59, 1);
      arduboy.setCursor(100, 57);
      arduboy.print(0);
      arduboy.fillTriangle(110, floor(loading), 107, floor(loading + 4), 120, loading, 1);
      arduboy.fillTriangle(110, floor(loading), 107, floor(loading - 4), 120, loading, 1);
      arduboy.display();
    }
    }
  */
}
void loop() {
  if (!arduboy.nextFrame()) return;
  if (arduboy.pressed(LEFT_BUTTON)) vehicleX = vehicleX - 2;
  if (arduboy.pressed(RIGHT_BUTTON)) vehicleX = vehicleX + 2;
  if (vehicleX < 20) vehicleX = 20;
  if (vehicleX > 108) vehicleX = 108;
  arduboy.pollButtons();
  if (arduboy.justPressed(UP_BUTTON)) gear = gear + 1;
  if (arduboy.justPressed(DOWN_BUTTON)) gear = gear - 1;
  if (gear < 1) gear = 1;
  if (gear > 5) gear = 5;
  if (arduboy.pressed(B_BUTTON)) {
    //rpm = velocity / gear * 75 + 825;
    //engine accelerates
    rpm = rpm + (100 / gear);//max((100 / gear) - (rpm / 50), 0);
    //limit engine RPM
    if (rpm > 5000) rpm = 5000;
    //clutch engaged, speed increase/decrease
    velocity = max((rpm - 825), 0) / 75 * gear;
  } else {
    //not applying gas, clutch disengaged
    //engine speed decrease
    rpm = rpm - 20;
    if (rpm <= 800) rpm = 800;
    //will decrease speed due to drag(related to speed)
    if (velocity > 0) velocity = velocity - round(velocity / 25);
    else velocity = 0;
  }
  if (arduboy.pressed(A_BUTTON)) {
    //if applying brake
    rpm = rpm - 20;
    if (rpm <= 800) rpm = 800;
    if (velocity > 0) velocity = velocity - 0.5;
    else velocity = 0;
  }
  arduboy.clear();
  track();
  arduboy.fillRect(vehicleX - 15, vehicleY - 5, 30, 10, 0);
  arduboy.drawRect(vehicleX - 15, vehicleY - 5, 30, 10, 1);
  //spash screen
  arduboy.setTextSize(1);
  arduboy.setCursor(2, 10);
  arduboy.print(rpm);
  arduboy.setCursor(32, 10);
  arduboy.print(velocity);
  //arduboy.setCursor(120, 10);
  //arduboy.print(life);

  //arduboy.setCursor(44, 10);
  //arduboy.print();
  //arduboy.setCursor(60, 10);
  //arduboy.print(vehicleX);
  //arduboy.setCursor(78, 10);
  //arduboy.print(vehicleY);
  arduboy.setCursor(96, 10);
  arduboy.print(gear);
  arduboy.setCursor(100, 25);
  arduboy.print(roadCurve);
  arduboy.setCursor(100, 35);
  arduboy.print(max(210 - velocity, 10));
  /*
    arduboy.setCursor(100, 45);
    arduboy.print(velocityX);
    arduboy.setCursor(100, 55);
    arduboy.print(velocityY);
    arduboy.setCursor(10, 50);
    arduboy.print(tick);
    arduboy.setCursor(10, 30);
    arduboy.print(arduboy.cpuLoad());
  *///splash screen
  arduboy.display();
}
