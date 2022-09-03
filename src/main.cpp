#include <Arduino.h>
#include <M5Unified.h>
#include <WiFiMulti.h>
#include <Avatar.h>
//#include <tasks/LipSync.h>
#include "ServoEasing.hpp"
#include "faces/bus_face.h"
#include <aquestalk.h>
#include "Env.h"

using namespace m5avatar;

//#define SERVO_PIN           5
#define NEOPIXEL_PIN        

//#define SERVO_START_DEGREE  90

Avatar avatar;
//ServoEasing servo;
ColorPalette cp;
WiFiMulti wifiMulti;

double pi = atan(1.0) * 4.0;

/// set M5Speaker virtual channel (0-7)
static constexpr uint8_t m5spk_virtual_channel = 0;

static constexpr uint8_t LEN_FRAME = 32;

static uint32_t workbuf[AQ_SIZE_WORKBUF];
static TaskHandle_t task_handle = nullptr;
volatile bool is_talking = false;


static void talk_task(void*)
{
  int16_t wav[3][LEN_FRAME];
  int tri_index = 0;
  for (;;)
  {
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY ); // wait notify
    while (is_talking)
    {
      uint16_t len;
      if (CAqTkPicoF_SyntheFrame(wav[tri_index], &len)) { is_talking = false; break; }
      M5.Speaker.playRaw(wav[tri_index], len, 8000, false, 1, m5spk_virtual_channel, false);
      tri_index = tri_index < 2 ? tri_index + 1 : 0;
    }
  }
}

/// 音声再生の終了を待機する;
static void waitAquesTalk(void)
{
  while (is_talking) { vTaskDelay(1); }
}

/// 音声再生を停止する;
static void stopAquesTalk(void)
{
  if (is_talking) { is_talking = false; vTaskDelay(1); }
}

/// 音声再生を開始する。(再生中の場合は中断して新たな音声再生を開始する) ;
static void playAquesTalk(const char *koe)
{
  stopAquesTalk();

  M5.Display.printf("Play:%s\n", koe);

  int iret = CAqTkPicoF_SetKoe((const uint8_t*)koe, 100, 0xFFu);
  if (iret) { M5.Display.println("ERR:CAqTkPicoF_SetKoe"); }

  is_talking = true;
  xTaskNotifyGive(task_handle);
}

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);
  //M5.begin();

  xTaskCreateUniversal(talk_task, "talk_task", 4096, nullptr, 1, &task_handle, APP_CPU_NUM);

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
#ifdef WIFI_SSID2
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
#endif
  // If you have more points, repeat the above three lines like WIFI_SSID3, 4, 5 ...

  //while (wifiMulti.run() != WL_CONNECTED) {
  //  delay(250);
  //  Serial.print(".");
  //  M5.Lcd.print(".");
  //}

  avatar.setRotation(pi);
  avatar.setFace(new BusFace());
  cp.set(COLOR_PRIMARY, TFT_WHITE);
  cp.set(COLOR_BACKGROUND, TFT_RED);
  cp.set(COLOR_SECONDARY, TFT_BLACK);
  avatar.setColorPalette(cp);
  avatar.init(1); // start drawing
  //avatar.addTask(lipSync, "lipSync");

  if (servo.attach(SERVO_PIN, SERVO_START_DEGREE)) {
    Serial.print("Error attaching servo x");
  }

  delay(500);

  int iret = CAqTkPicoF_Init(workbuf, LEN_FRAME, AQUESTALK_KEY);
  if (iret) {
    M5.Display.println("ERR:CAqTkPicoF_Init");
  }

  playAquesTalk("do'-mo.ba'_su/ta'xtu_kutyan;de_su.");
  waitAquesTalk();
  //playAquesTalk("akue_suto'-_ku/kido-shima'_shita.");
  //waitAquesTalk();
  playAquesTalk("botanno/o_shitekudasa'i.");
}

bool moving = false;
bool pressed = false;

void loop()
{
  M5.update();

  if (     M5.BtnA.wasClicked())  { playAquesTalk("kuri'kku"); }
  else if (M5.BtnA.wasHold())     { playAquesTalk("ho'-rudo"); }
  else if (M5.BtnA.wasReleased()) { playAquesTalk("riri'-su"); }
  else if (M5.BtnB.wasReleased()) { playAquesTalk("korewa;te'_sutode_su."); }
  else if (M5.BtnC.wasReleased()) { playAquesTalk("yukkuri_siteittene?"); }
}
