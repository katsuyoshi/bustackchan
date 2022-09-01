#include <M5Unified.h>
#include <Avatar.h>
#include "ServoEasing.hpp"
#include "faces/bus_face.h"

using namespace m5avatar;

#define SERVO_PIN           5

#define SERVO_START_DEGREE  90

Avatar avatar;
ServoEasing servo;
ColorPalette cp;

double pi = atan(1.0) * 4.0;

void setup()
{
  M5.begin();
  //avatar.setPosition(50, 50);
  //avatar.setRotation(PI * 180.00003 / 180.0);
  avatar.setRotation(pi);
  avatar.setFace(new BusFace());
  cp.set(COLOR_PRIMARY, TFT_WHITE);
  cp.set(COLOR_BACKGROUND, TFT_RED);
  cp.set(COLOR_SECONDARY, TFT_BLACK);
  avatar.setColorPalette(cp);
  avatar.init(8); // start drawing

  if (servo.attach(SERVO_PIN, SERVO_START_DEGREE)) {
    Serial.print("Error attaching servo x");
  }
  delay(500);
}

bool moving = false;
bool pressed = false;

void loop()
{
  M5.update();
  if (M5.BtnB.wasPressed()) {
    moving = !moving;
  }

  int f = 15;//65;
  int r = 15;//55;
  if (moving) {
    servo.easeTo(SERVO_START_DEGREE + f, 40); // Blocking call
    delay(1000);
    servo.easeTo(SERVO_START_DEGREE - r, 40); // Blocking call
    delay(1000);
  } else {
    Serial.println(F("Move to 0 degree  with 40 degree per second blocking"));
    servo.easeTo(SERVO_START_DEGREE, 40); // Blocking call
    delay(1000);
  }
}
