#include <Arduino.h>
#include <M5Unified.h>
#include <WiFiMulti.h>
#include <Avatar.h>
#include <NeoPixelBus.h>
//#include <tasks/LipSync.h>
#include "ServoEasing.hpp"
#include "faces/bus_face.h"
#include <aquestalk.h>
#include "Env.h"

using namespace m5avatar;

//#define SERVO_PIN           5
#define PIXEL_PIN             2

//#define SERVO_START_DEGREE  90

typedef enum led_pattern {
  LPNone,
  LPLeft,
  LPRight,
  LPStop,
};



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

volatile bool boarding = false;
int next_time = 0;

static led_pattern led_pat = LPNone;

const uint16_t PixelCount = 15;
const uint8_t AnimationChannels = 1;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PIXEL_PIN);

#define colorSaturation 128

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor yellow(colorSaturation, colorSaturation, 0);
RgbColor orange(colorSaturation, colorSaturation * 165 / 255, 0);
RgbColor white(colorSaturation);
RgbColor black(0);

typedef enum phrase_id {
  PHGreeting,
  PHJousha,
  PHOriru,
  PHSutakuNumaYuki,
  PHGrooto,
  PHArigato,
  PHTugiwa,
};

static phrase_id speaking_phrase = PHGreeting;

static const char *phrases[] = {
  "do'-mo.ba'_su/ta'xtu_kutyan;de_su.",
  "onorino+katawa/botan;o/o_sitekudasa'i.",
  "oorinokatahabo'tan;o/o_sitekudasa'i.",
  "kono/ba'suwa/_suttuku'numa/yu'kide_su.",
  "kono/ba'suwa/a_kitake'n;naio/syu-kaisuru/guru'ttode_su.",
  "gozyo-sya/ari'gato-+gozaima'_sita.",
  "tugi'wa.",
};

static const char *municipalities[] = {
  "higasinarusemura",
  "yuzawa'_si.",
  "ugo'mati",
  "yokote'_si.",
  "misato'tyo-",
  "daise'n;_si.",
  "sen;boku'_si.",
  "kazuno'_si.",
  "kosaka'mati",
  "o-date'_si.",
  "huzisato'mati",
  "happou'tyo-",
  "nosiro'_si.",
  "_kitaa_kita'_si.",
  "kamikoanimura",
  "mitane'tyo-",
  "oogatamura",
  "oga'_si.",
  "katagami'_si.",
  "ikawa'mati",
  "hatiro-gata'mati",
  "gozyo-me'mati",
  "a_kita'_si.",
  "yurihon;zyo'-_si.",
  "nikaho'si"
};


static void set_left_led() {
  for (int i = 0; i < PixelCount; i++) {
    if (i < 6) {
      strip.SetPixelColor(i, yellow);
    } else {
      strip.SetPixelColor(i, black);
    }
  }
  strip.Show();
}

static void set_right_led() {
  for (int i = 0; i < PixelCount; i++) {
    if (i >= 9) {
      strip.SetPixelColor(i, yellow);
    } else {
      strip.SetPixelColor(i, black);
    }
  }
  strip.Show();
}

static void set_stop_led() {
  for (int i = 0; i < PixelCount; i++) {
    if (i >= 6 && i < 9) {
      strip.SetPixelColor(i, red);
    } else {
      strip.SetPixelColor(i, black);
    }
  }
  strip.Show();
}

static void set_blank_led() {
  for (int i = 0; i < PixelCount; i++) {
    strip.SetPixelColor(i, black);
  }
  strip.Show();
}

static void led_task(void*)
{
  strip.Begin();
  while(true) {
    switch(led_pat) {
      case LPLeft:
        set_left_led();
        vTaskDelay(500);
        break;
      case LPRight:
        set_right_led();
        vTaskDelay(500);
        break;
      case LPStop:
        set_stop_led();
        vTaskDelay(500);
        break;
      default:
        set_blank_led();
        vTaskDelay(500);
        break;
    }

    led_pat = led_pattern((led_pat + 1) % 4);
  }
}

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

static void lip_sync_task(void *args)
{
  DriveContext *ctx = reinterpret_cast<DriveContext *> (args);
  Avatar *avatar = ctx->getAvatar();
  while(true) {
    if (is_talking) {
      float open = random(0,99) / 100.00;
      avatar->setMouthOpenRatio(open);
    } else {
      avatar->setMouthOpenRatio(1.0);
    }
    vTaskDelay(5);
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

  //M5.Display.printf("Play:%s\n", koe);

  int iret = CAqTkPicoF_SetKoe((const uint8_t*)koe, 100, 0xFFu);
  //if (iret) { M5.Display.println("ERR:CAqTkPicoF_SetKoe"); }

  is_talking = true;
  xTaskNotifyGive(task_handle);
}

void speak(phrase_id id) {
  speaking_phrase = id;
  playAquesTalk(phrases[id]);
}

void speak_next_station() {
  int i = next_time % 24;
  if (i == 23) {
    speak(PHGrooto);
  } else {
    speak(PHTugiwa);
    waitAquesTalk();
    playAquesTalk(municipalities[i]);
    waitAquesTalk();
    vTaskDelay(500);
    speak(PHOriru);
  }
}



time_t time_synced_at = 0;
uint32_t millis_since_time_synced;

const char* ntpServer = "ntp.nict.jp";
const long  gmtOffset_sec = 3600 * 9;
const int   daylightOffset_sec = 0;


// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void syncTime() {
  configTime(0, 0, "pool.ntp.org");
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    vTaskDelay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
  Serial.print(timeinfo.tm_year);

  time_synced_at = mktime(&timeinfo);
  millis_since_time_synced = millis();
}

time_t now() {
  time_t n = (time_t)((millis() + 9 * 3600 * 1000 - (time_t)millis_since_time_synced) / 1000) + time_synced_at;
  return n;
}

int interval = 1;

int now_time() {
  time_t n = now();
  struct tm *t = gmtime(&n);
  return t->tm_hour * (60 / interval)  + t->tm_min / interval;
}

void update_next_time() {
  next_time = (now_time() + 1) % (24 * 60 / interval);
  Serial.printf("next_time: %d\n", next_time);
}



void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  xTaskCreateUniversal(talk_task, "talk_task", 4096, nullptr, 1, &task_handle, APP_CPU_NUM);

  M5.Speaker.setVolume(64); //128); // 30);

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

  //if (servo.attach(SERVO_PIN, SERVO_START_DEGREE)) {
  //  Serial.print("Error attaching servo x");
  //}

  delay(500);

  int iret = CAqTkPicoF_Init(workbuf, LEN_FRAME, AQUESTALK_KEY);
  if (iret) {
    M5.Display.println("ERR:CAqTkPicoF_Init");
  }

  xTaskCreatePinnedToCore(led_task, "led", 2048, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(lip_sync_task, "lip_sync", 1024, NULL, 1, NULL, 1);
  avatar.addTask(lip_sync_task, "lipSync");

  speak(PHGreeting);
  waitAquesTalk();
}

void loop()
{
  if (time_synced_at == 0) {
    if (wifiMulti.run() == WL_CONNECTED) {
      syncTime();
      update_next_time();
    }
  } else {
    if (now_time() == next_time) {
      if (boarding) {
        speak_next_station();
      } else {
        speak(PHJousha);
      }
      update_next_time();
    } else {
      Serial.printf("now_time: %d\n", now_time());
      delay(1000);
    }
  }


  M5.update();

  if (boarding) {
    if (M5.BtnA.wasClicked()) {
      boarding = !boarding;
      speak(PHArigato);
    } else
    if (M5.BtnC.wasClicked()) {
      boarding = !boarding;
      speak(PHArigato);
    }
  } else {
    if (M5.BtnA.wasClicked()) {
      boarding = !boarding;
      speak(PHSutakuNumaYuki);
    } else
    if (M5.BtnC.wasClicked()) {
      boarding = !boarding;
      speak(PHGrooto);
    }
  }
  if (M5.BtnB.wasReleased()) { speak(PHGreeting); }
}
