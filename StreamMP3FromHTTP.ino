#include <Arduino.h>

#include <WiFi.h>

#define ESP32

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputPimoroni.h"

// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.

// Enter your WiFi setup here:
#ifndef STASSID
#define STASSID "........."
#define STAPSK ".........."
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Randomly picked URL
const char *allUrls[] = {
  // URL does not exist, only here when need to test disconnects/bad urls/etc.
  // "http://stream1.wfmu.org/freeform-128k",
  "http://stream0.wfmu.org/freeform-128k",
  "http://stream2.wfmu.org/freeform-128k",
  "http://stream3.wfmu.org/freeform-128k",
};
#define URLS_LENGTH 3

AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputPimoroni *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);

  // Try forever
  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    if(connectionAttempts++ > 500) {
      rp2040.reboot();
    }
    delay(1000);
  }
  Serial.println("Connected");

  audioLogger = &Serial;
  int urlIndex = rp2040.hwrand32() % URLS_LENGTH;
  file = new AudioFileSourceICYStream(allUrls[urlIndex]);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  buff = new AudioFileSourceBuffer(file, 2048);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  out = new AudioOutputPimoroni();
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);
}


void loop()
{
  static int lastms = 0;
  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.println("MP3 done\n");
    delay(5000);
    Serial.println("rebooting");

    rp2040.reboot();
  }
}
