#include <Arduboy2.h>
Arduboy2 arduboy;
byte hflip;
byte gear;
byte gameTimer;
byte speed;
byte score;
uint16_t speedAcc;
int16_t playerX;
int16_t playerY;
int16_t currentRoadCurve;
int16_t desiredRoadCurve;
int16_t nextRoadCurveChange;
int16_t backgroundXOffset;
int8_t trackCenteredX;
int8_t puffX;
int8_t puffY;
int8_t puffFrame;

int16_t enemyX;
uint16_t enemyYAcc;
byte enemySpeed;

// Maximum number of simultaneously tree/road-side object
#define MAX_TREE 8
// Y position of palm trees
uint16_t palmYAcc[MAX_TREE];
// State of the object: 0=none, 1=left palm tree, 2=right palm tree, etc.
byte palmState[MAX_TREE];
// Counter to determine when the next palm tree should appear
uint16_t nextPalm;

byte trackPixWidth[44];
int16_t trackStartX[44];

const byte PROGMEM yLookup[256] = { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, };

void drawOutrunTrack() {
  uint16_t trackWidth = 1 << 7;
  //uint16_t trackIncrement = 128 + (y << 4);
  uint16_t trackIncrement = 128 + (47 << 4);
  int16_t trackCenter = 64 << 4;

  uint16_t roadLineAcc = 0;

  if (gameTimer <= 99) {
    speedAcc += speed;
  }

  roadLineAcc = 8192 - (speedAcc << 6);

  int16_t roadCurve = (currentRoadCurve >> 1);
  trackCenter += roadCurve >> 2;
  //byte yMult = 139;
  //int8_t roadCurveInc = ((currentRoadCurve >> 8) * yMult) >> 8;
  int8_t roadCurveInc = currentRoadCurve / 118;
  int8_t newCenteredX = (playerX >> 10);
  if (trackCenteredX < newCenteredX && trackCenteredX < 45) {
    trackCenteredX++;
  }
  else if (trackCenteredX > newCenteredX && trackCenteredX > -45) {
    trackCenteredX--;
  }
  //byte xMult = 120;
  //int8_t xViewShift = (trackCenteredX * xMult + 128) >> 8;

  for (byte i = 20; i < 64; ++i) {
    //arduboy.drawFastHLine(0, i, 127, 1);

    byte pixWidth = trackWidth >> 8;
    trackPixWidth[i - 20] = pixWidth;

    int16_t startx = (trackCenter - (int16_t)(trackWidth >> 4)) >> 4;
    trackStartX[i - 20] = startx;

    byte sideline = (pixWidth >> 4) + 1;
    byte road = pixWidth - ((sideline * 3 + 1) >> 1);
    // using this if, we skip drawing completely if the HLine would be color 2 (gray), since the screen is pre-filled with gray
    if (roadLineAcc & 16384) {
      arduboy.drawFastHLine(0, i, startx, 1);
    }

    arduboy.drawFastHLine(startx, i, sideline, roadLineAcc & 8192 ? 1 : 0);
    startx += sideline;
    //arduboy.drawFastHLine(startx, i, road, 2);
    startx += road;

    if (roadLineAcc & 16384) {
      arduboy.drawFastHLine(startx, i, sideline, 1);
    }
    startx += sideline;

    //arduboy.drawFastHLine(startx, i, road, 2);
    startx += road;

    arduboy.drawFastHLine(startx, i, sideline, roadLineAcc & 8192 ? 1 : 0);
    startx += sideline;

    if (roadLineAcc & 16384) {
      arduboy.drawFastHLine(startx, i, 256, 1);
    }

    //roadLineAcc += (45184 - trackWidth) >> 3;
    roadLineAcc += uint16_t(60000) / byte(6 + (trackWidth >> 9));

    trackWidth += trackIncrement;
    trackCenter -= trackCenteredX + ((-roadCurveInc + (roadCurve >> 4)) >> 2);
    roadCurve -= roadCurveInc;
  }
  // perspective: 
  // at horizon (20), track width should be 4
  // at bottom (63), track width should be 220

  //uint16_t trackWidth = 4 << 7;
  //uint16_t trackIncrement = 128;
  //int16_t trackCenter = 64 << 4;
  //static uint16_t stripOffset = 6912;
  //uint16_t stripSize = (stripOffset + (24000 / y) * (y - 32)) & 8191;

  //byte alternateStrip = 0;
  //arduboy.setCursor(90, 40);
  //arduboy.print(stripOffset);

  //static uint16_t speed = 0;

  ///*
  //a y=26, strip 1280 (938 par y)
  //a y=30, strip 5120 (896 par y)
  //a y=32, strip 6912 reference
  //a y=34, strip 256  (768 par y)
  //a y=36, strip 1280 (640 par y)
  //a y=40, strip 3584 (608 par y)
  //*/

  //if (arduboy.pressed(A_BUTTON)) {
  //  speed--;
  //}
  //if (arduboy.pressed(B_BUTTON)) {
  //  speed++;
  //}

  //stripOffset += speed;
  //stripOffset = stripOffset & 8191;

  //for (byte i = 20; i < 64; ++i) {
  //  uint16_t pixWidth = trackWidth >> 7;
  //  //arduboy.drawFastHLine((trackCenter >> 4) - (pixWidth >> 1), i, pixWidth, 1);
  //  uint16_t startx = (trackCenter >> 4) - (pixWidth >> 1);
  //  uint16_t sideline = (pixWidth >> 4) + 1;
  //  uint16_t road = (pixWidth - sideline * 3) >> 1;
  //  arduboy.drawFastHLine(startx, i, sideline, 1);
  //  startx += sideline;
  //  arduboy.drawFastHLine(startx, i, road, 0);
  //  startx += road;
  //  if (alternateStrip) {
  //    arduboy.drawFastHLine(startx, i, sideline, 1);
  //  }
  //  startx += sideline;
  //  arduboy.drawFastHLine(startx, i, road, 0);
  //  startx += road;
  //  arduboy.drawFastHLine(startx, i, sideline, 1);

  //  trackWidth += trackIncrement;
  //  trackIncrement += (y);

  //  trackCenter += (x - 64);

  //  stripSize += 2048;
  //  if (stripSize > trackWidth) {
  //    alternateStrip ^= 1;
  //    stripSize -= trackWidth;
  //  }
  //}
}


































void UpdatePalm() {
  if (gameTimer > 99) return;

  if (nextPalm < speed) {
    // Spawn a new tree by checking the first available slot
    for (byte i = 0; i < MAX_TREE; i++) {
      if (palmState[i] == 0) {
        palmState[i] = random(2) + 1;
        palmYAcc[i] = 0;
        nextPalm = 100 + random(1400);
        break;
      }
    }
  }
  else {
    nextPalm -= speed;
  }

  for (byte i = 0; i < MAX_TREE; i++) {
    if (palmState[i] == 0) continue;

    // Make tree move according to player current speed
    palmYAcc[i] += speed;
    // Check if tree would go off the bottom of the screen: if so, free its slot
    uint16_t tempy = (palmYAcc[i] >> 3);
    if (tempy > 255) {
      palmState[i] = 0;
      continue;
    }

    // Calculate perspective y using lookup table
    byte palmy = pgm_read_byte(yLookup + byte(tempy));
    // Determine which sprite scale, depending on hard-coded y values:
    const byte* whichSprite2 = palmy > 43 ? palm_1 : palmy > 32 ? palm_2 : palm_3;

    // Draw test palm tree on side of the road, left or right:
    int16_t palmx = trackStartX[palmy - 20];
    if (palmState[i] == 1) {
      palmx -= 8;
      if (palmx >= 0) {
        arduboy.drawMaskBitmap(palmx, palmy, whichSprite2, false);
      }
    }
    else {
      palmx += 8 + (trackPixWidth[palmy - 20] << 1);
      if (palmx <= 136) {
        arduboy.drawMaskBitmap(palmx, palmy, whichSprite2, true);
      }
    }
  }
}














void setup() {
arduboy.boot();//fast boot, why not?
arduboy.setFrameRate(30);
}
void loop() {
  drawOutRunTrack();

  UpdatePalm();
}

