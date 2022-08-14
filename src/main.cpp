#include <M5Unified.h>
#include <Avatar.h>

using namespace m5avatar;

M5GFX display;

Avatar avatar;

void setup()
{
  M5.begin();
  avatar.init(); // start drawing
  avatar.setRotation(PI);
}

void loop()
{
  // avatar's face updates in another thread
  // so no need to loop-by-loop rendering
}
