/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

  *****************************************************************************************************************************/

#include <Arduino.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE // this overrides CONFIG_LOG_MAXIMUM_LEVEL setting in menuconfig
                                        // and must be defined before including esp_log.h
#define MY_GLOBAL_DEBUG_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "defines.h"
#include "Credentials.h"

#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);

#include "index.h"

#include "ESPAsync_WiFiManager_Lite.h"
ESPAsync_WiFiManager_Lite *ESPAsync_WiFiManager;
bool _drdStopped = false;

// SOME DEFINES
#include "tinyDefs.h"

// LVGL
#define WT_USE_LVGL
#include "WT32_SC01_PLUS.h"

SemaphoreHandle_t lvgl_mux;

// ENCODER
// SparkOfCode: include library only if needed
#if ( defined(USE_ENCODER_VOLUME) || defined(USE_ENCODER_TUNE) )
#include "AiEsp32RotaryEncoder.h"
#define ROTARY_ENCODER_STEPS 4
#endif

// VOLUME_ENCODER
#ifdef USE_ENCODER_VOLUME
AiEsp32RotaryEncoder volumeEncoder = AiEsp32RotaryEncoder(ENC_VOLUME_A_PIN, ENC_VOLUME_B_PIN, ENC_VOUME_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

void IRAM_ATTR volume_readEncoderISR()
{
  volumeEncoder.readEncoder_ISR();
}

#endif

// TUNE_ENCODER
#ifdef USE_ENCODER_TUNE
AiEsp32RotaryEncoder tuneEncoder = AiEsp32RotaryEncoder(ENC_TUNE_A_PIN, ENC_TUNE_B_PIN, ENC_TUNE_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

void IRAM_ATTR tune_readEncoderISR()
{
  tuneEncoder.readEncoder_ISR();
}
#endif

// SparkOfCode: Volume Potentiometer
#ifdef USE_POT_VOLUME
uint16_t volPotValue;
uint16_t volPotValueNew;
#endif

// GUI

#include "RetroGUI.h"
RetroGUI GUI;

// #include "SimpleGUI.h"
// SimpleGUI GUI;

// STATIONS
#include "Stations.h"
static Stations tinyStations;
TinyStations stations(tinyStations);

// AUDIO
#include "Audio.h" //wolle aka schreibfaul1
#include "audiotask.h"
bool _isPaused = false;

#include <Preferences.h>
Preferences preferences;

#include <ArduinoJson.h>

JsonDocument doc;
String jsonString;

const char defaultJson[] PROGMEM = R"=====(
{"colorBackground":"#000000", 
"colorStation":"#ffa500",
"colorMarker":"#e3e700",
"colorTuning":"#e32400",
"colorVolume":"#6aff12",
"stations":[
{"shortName":"RADIO.EINS",
"StreamURL":"http://www.radioeins.de/livemp3"},
{"shortName":"COSTA.D.MAR",
"StreamURL":"http://radio4.cdm-radio.com:8020/stream-mp3-Chill_autodj"},
{"shortName":"Kiss.FM",
"StreamURL":"http://topradio-stream31.radiohost.de/kissfm_mp3-128"},
{"shortName":"paradise",
"StreamURL":"http://stream-uk1.radioparadise.com/aac-320"}
]}
)=====";

uint8_t _lastVolume = 5;
uint8_t _lastStation = 0;
uint16_t _vum = 0;

uint8_t _maxVolume = 21;
uint8_t _num_stations = 1;

// FOR vTask_delay
uint32_t ms;

lv_obj_t *tempLabel;
LV_FONT_DECLARE(Berlin25_4);

// ****************** PREFERENCES (FOR LAST STATION PLAYED && LAST VOLUME *******************************************
void savePrefs()
{
  preferences.begin(PREFS_NAME, false);
  preferences.putLong(LAST_VOLUME, _lastVolume);
  preferences.putLong(LAST_STATION, _lastStation);
  preferences.end();
  LV_LOG_USER("SETTINGS SAVED");
}

void loadPrefs()
{
  preferences.begin(PREFS_NAME, false);
  _lastVolume = preferences.getLong(LAST_VOLUME, 3);
  if (_lastVolume > VOLUME_STEPS)
  {
    _lastVolume = VOLUME_STEPS / 2;
  } // HALF OF MAX
  _lastStation = preferences.getLong(LAST_STATION, 0);
  preferences.end();
  LV_LOG_USER("SETTINGS LOADED");
}

// ****************** UI SETTINGS (COLORS, STATIONS, ..) *******************************************

String loadJSON()
{
  LV_LOG_USER("LOAD JSON");
  File file = LittleFS.open("/Settings.json", "r");
  if (file)
  {
    String ret = file.readString();
    LV_LOG_USER("FILE CLOSE");
    file.close();
    LV_LOG_USER("FILE CLOSED");
    if (ret != "")
    {
      // GOT IT !
      return ret;
    }
  }
  LV_LOG_USER("DEFAULT JSON");
  return defaultJson; // IF NOT FOUND
}

String loadSettings() // ONLY FOR WEBSERVER-CONFIG
{
  LV_LOG_USER("LOAD SETTINGS");
  File file = LittleFS.open("/Settings.json", "r");
  String payload = "";

  if (file)
  {
    LV_LOG_USER("FILE FOUND");
    payload = file.readString();

    LV_LOG_USER("FILE CLOSE");
    file.close();
    LV_LOG_USER("FILE CLOSED");

    if (payload != "")
    {
      stations.loadFromJson(tinyStations, payload);
      _num_stations = tinyStations.size();

      if (_lastStation > _num_stations - 1)
      {
        _lastStation = _num_stations - 1;
      }

      String ret = "var jsondata = ";
      ret += payload;
      // return index_PayLoad; //ONLY FOR TESTING
      return ret;
    }
  }
  LV_LOG_USER("NO FILE FOUND - USING DEFAULTS.");
  return index_PayLoad;
}

bool saveSettings(String payload)
{
  // UPDATE GUI

  stations.loadFromJson(tinyStations, payload);
  _num_stations = tinyStations.size();

  if (_lastStation > _num_stations - 1)
  {
    _lastStation = _num_stations - 1;
  }

  // SAVE IT
  bool ret = false;
  File file = LittleFS.open("/Settings.json", "w", true);
  if (file)
  {
    size_t bytesWritten = file.write((const uint8_t *)payload.c_str(), (size_t)payload.length());
    if (bytesWritten == payload.length())
    {
      ret = true;
    }
    LV_LOG_USER("FILE CLOSE");
    file.close();
    LV_LOG_USER("FILE CLOSED");

    // UPDATE GUI
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.setStations(tinyStations);
    GUI.update(payload);
    GUI.tuneToStation(_lastStation);
    GUI.setVolumeIndicator(_lastVolume);
    GUI.updateDRDindicator(_drdStopped);
    xSemaphoreGiveRecursive(lvgl_mux);
    audioConnecttohost(tinyStations[_lastStation].URL.c_str());
    LV_LOG_USER("GUI UPDATED");
  }
  else
  {
    LV_LOG_USER("CANNOT WRITE FILE");
  }
  return ret;
}

// ****************** CALLBACKS FROM SETTINGS-WEBSERVER *******************************************

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void gotSettings(AsyncWebServerRequest *request)
{

  int params = request->params();
  for (int i = 0; i < params; i++)
  {
    // SparkOfCode: added "const" to avoid invalid conversion error
    const AsyncWebParameter *p = request->getParam(i);
    if (p->name() == "Settings")
    {
      saveSettings(p->value());
    }
  }
  request->redirect("/");
}

void onRoot(AsyncWebServerRequest *request)
{

  LV_LOG_USER("Web Server: New request received: => /"); // for debugging

  String html = index_partOne;
  html += loadSettings();
  html += index_partTwo;
  request->send(200, "text/html", html);
}

// ****************** LVGL  TASK ***********************************
TaskHandle_t Task_lvgl;

void lvglTask(void *parameter)
{
  // LVGL
  while (true)
  {
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    ms = lv_timer_handler();
    xSemaphoreGiveRecursive(lvgl_mux);
    vTaskDelay(pdMS_TO_TICKS(ms));
  }
}

void createLVGL_Task()
{
  xTaskCreatePinnedToCore(
      lvglTask,       /* Function to implement the task */
      "lvglTask",     /* Name of the task */
      10000,          /* Stack size in words */
      NULL,           /* Task input parameter */
      LVGL_TASK_PRIO, /* Priority of the task */
      &Task_lvgl,     /* Task handle. */
      LVGL_TASK_CORE  /* Core where the task should run */
  );
}

void lvglTaskDelete()
{
  vTaskDelete(Task_lvgl);
}

// ****************** VU-METER  TASK ***********************************
TaskHandle_t Task_vu;

void vuTask(void *parameter)
{
  // vuMeter
  while (true)
  {
    // UPDATE VU-METER (IF ANY)
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    if (_isPaused)
    {
      GUI.setVUMeterValue(0);
    }
    else
    {
      GUI.setVUMeterValue(_vum);
    }
    xSemaphoreGiveRecursive(lvgl_mux);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void createVU_Task()
{
  xTaskCreatePinnedToCore(
      vuTask,        /* Function to implement the task */
      "vuTask",      /* Name of the task */
      10000,         /* Stack size in words */
      NULL,          /* Task input parameter */
      20,            /* Priority of the task */
      &Task_vu,      /* Task handle. */
      LVGL_TASK_CORE /* Core where the task should run */
  );
}

void vuTaskDelete()
{
  vTaskDelete(Task_vu);
}

// TODO:
#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] PROGMEM =
    "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}"
    "button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

// SparkOfCode: Encoder as state machine
#ifdef USE_ENCODER_TUNE_SM
int encoderChanged() // char used for 8-bit signed integer; -1: counter-clockwise; 0: no change; 1: clockwise
{
  static uint8_t state = 0;
  bool CLKstate = digitalRead(ENC_TUNE_B_PIN);
  bool DTstate = digitalRead(ENC_TUNE_A_PIN);
  switch (state)
  {
  case 0: // Coming from idle state, encoder not turning
    if (!CLKstate)
    { // Turn clockwise: CLK goes low first
      state = 1;
    }
    else if (!DTstate)
    { // Turn anticlockwise: DT goes low first
      state = 4;
    }
    return 0;
  case 1: // Clockwise rotation phase 2
    if (!DTstate)
    { // Continue clockwise: DT will go low after CLK
      state = 2;
    }
//    return 0;
  case 2: // Clockwise rotation phase 3: turn further and CLK will go high first
    if (CLKstate)
    {
      state = 3;
    }
    return 0;
  case 3: // Clockwise rotation phase 4: both CLK and DT now high as the encoder completes one step clockwise
    if (CLKstate && DTstate)
    {
      state = 0;
      return 1;
    }
    return 0;
  case 4: // Anticlockwise: CLK and DT reversed
    if (!CLKstate)
    {
      state = 5;
    }
    return 0;
  case 5:
    if (DTstate)
    {
      state = 6;
    }
    return 0;
  case 6:
    if (CLKstate && DTstate)
    {
      state = 0;
      return -1;
    }
    return 0;
  };
  return 0;
}
#endif

void setup()
{
  
  // Debug console
  static const char *TAG = "SETUP";

  // Serial.begin(115200);
  // delay(3000);                  //NEEDED BECAUSE OF USB-C
  // Serial.setDebugOutput(true);
  // Serial.flush();

  esp_log_level_set("*", ESP_LOG_VERBOSE);

  LittleFS.begin(true, "/LittleFS");

  //**************************************************************************
  //                               LVGL
  //**************************************************************************
  lvgl_mux = xSemaphoreCreateRecursiveMutex();
  init_display();
  lv_tick_set_cb(xTaskGetTickCount); // LVGL TICK

  // START LVGL TASK
  createLVGL_Task();

  // WE NEED A TEMPORARY MESSAGE (IF SOMETHING IS TO KNOW)
  tempLabel = lv_label_create(lv_scr_act());
  lv_obj_set_size(tempLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT); // 90% WIDTH OF PARENT
  lv_obj_align(tempLabel, LV_ALIGN_CENTER, 0, 0);               // CENTER ON PARENT
  lv_obj_set_style_text_font(tempLabel, &Berlin25_4, 0);        // CUSTOM FONT
  lv_obj_set_style_text_align(tempLabel, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_clear_flag(tempLabel, LV_OBJ_FLAG_SCROLLABLE); // DON'T USE SCROLLBARS
  String ss = "Starting ..";
  // SHOW IT

  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  lv_label_set_text(tempLabel, ss.c_str());
  xSemaphoreGiveRecursive(lvgl_mux);
  vTaskDelay(pdMS_TO_TICKS(1000));

  //**************************************************************************
  //                               WiFi-MANAGER_LITE
  //**************************************************************************

  // bool _isWifi = wifimanager_begin();

  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "Starting ESPAsync_WiFi");

#if USING_MRD
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
#else
  ESP_LOGI(TAG, ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  String AP_SSID = "TinyRadio";
  String AP_PWD = "12345678"; // EMPTY PASSWORDS ARE FORBIDDEN :-()
  ESPAsync_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);

#if USING_CUSTOMS_STYLE
  ESPAsync_WiFiManager->setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
  ESPAsync_WiFiManager->setCustomsHeadElement(PSTR("<style>html{filter: invert(10%);}</style>"));
#endif

#if USING_CORS_FEATURE
  ESPAsync_WiFiManager->setCORSHeader(PSTR("Your Access-Control-Allow-Origin"));
#endif

  String msg = "Verbinde mit WLAN ... ";

  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  lv_label_set_text(tempLabel, msg.c_str());
  xSemaphoreGiveRecursive(lvgl_mux);

  ESPAsync_WiFiManager->begin(HOST_NAME);

  // CHECK STATUS
  bool _connected = false;
  while (!_connected)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      _connected = true;
    }
    else
    {
      if (ESPAsync_WiFiManager->isConfigMode())
      {
        msg = "Bitte mit WLAN \n* ";
        msg += HOST_NAME;
        msg += " *\n VERBINDEN";
        xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
        lv_label_set_text(tempLabel, msg.c_str());
        xSemaphoreGiveRecursive(lvgl_mux);
      }
    }
  }
  //**************************************************************************
  //                               SETTINGS - SERVER
  //**************************************************************************

  ESP_LOGI(TAG, "CONNECTED TO WiFi...");

  if (!MDNS.begin(HOST_NAME))
  {
    ESP_LOGE(TAG, "Error starting mDNS");
  }
  else
  {
    MDNS.addService("http", "tcp", 80);
    ESP_LOGI(TAG, "mDNS name: %s", HOST_NAME);
  }
  // SET CALLBACKS FOR CONFIG PORTAL & START IT
  server.on("/", HTTP_GET, onRoot);
  server.on("/save", HTTP_POST, gotSettings);
  server.onNotFound(notFound);
  server.begin();

  //**************************************************************************
  //                              GUI
  //**************************************************************************
  // WE DONN'T NEED IT ANYMORE
  if (tempLabel)
  {
    lv_obj_delete(tempLabel);
  }

  // SCREEN BLACK
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

  // STATIONS, COLORS ..
  String jsonSettings = loadJSON();
  stations.loadFromJson(tinyStations, jsonSettings);
  _num_stations = tinyStations.size();

  // LAST_VOLUME, LAST_STATION
  loadPrefs();

  // LAST_VOLUME
  if (_lastVolume > _maxVolume)
  {
    _lastVolume = _maxVolume;
  }
  if (_lastVolume < 0)
  {
    _lastVolume = 0;
  }

  // LAST STATION
  if (_lastStation < 0)
  {
    _lastStation = 0;
  }
  if (_lastStation > _num_stations)
  {
    _lastStation = _num_stations - 1;
  }

  savePrefs(); // TO BE SURE ...

  // PUSH TO GUI
  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  GUI.setStations(tinyStations);
  GUI.begin(lv_scr_act(), jsonSettings);
  xSemaphoreGiveRecursive(lvgl_mux);

  //**************************************************************************
  //                               AUDIO
  //**************************************************************************

  // LET AUDIO DO ITS WORK - TASK ON CORE 1
  audioInit();

  audioSetMaxVolume(VOLUME_STEPS); // tinyDefs.h
  _maxVolume = audioGetMaxVolume();
  GUI.setVolumeIndicatorMax(_maxVolume);

  audioSetVolume(_lastVolume);
  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  GUI.setVolumeIndicator(_lastVolume);
  xSemaphoreGiveRecursive(lvgl_mux);

// ENCODER
#ifdef USE_ENCODER_VOLUME
  // we must initialize rotary encoder
  volumeEncoder.begin();
  volumeEncoder.setup(volume_readEncoderISR);
  // set boundaries and if values should cycle or not
  bool circleValues_volume = false;
  volumeEncoder.setBoundaries(0, VOLUME_STEPS, circleValues_volume); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  volumeEncoder.disableAcceleration();
  volumeEncoder.setEncoderValue(_lastVolume);
#endif

#ifdef USE_ENCODER_TUNE
  // we must initialize rotary encoder
  tuneEncoder.begin();
  tuneEncoder.setup(tune_readEncoderISR);
  // set boundaries and if values should cycle or not
  bool circleValues_tune = false;
  tuneEncoder.setBoundaries(0, TFT_HOR_RES - POINTER_WIDTH * 1.5, circleValues_tune); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  // tuneEncoder.disableAcceleration();
  tuneEncoder.setEncoderValue(GUI.getStationMidX(_lastStation));
#endif

  // SparkOfCode: Tune encoder pins for encoders with or without pullup resistors
  #ifdef USE_ENCODER_TUNE_SM // Tune encoder implemented as state machine
    #ifdef USE_ENC_TUNE_INTERNAL_PULLUP
      pinMode(ENC_TUNE_A_PIN, INPUT_PULLUP); // use for encoder without pullups
      pinMode(ENC_TUNE_B_PIN, INPUT_PULLUP); // use for encoder without pullups
    #else
      pinMode(ENC_TUNE_A_PIN, INPUT); // use for encoder (module) with pullups
      pinMode(ENC_TUNE_B_PIN, INPUT); // use for encoder (module) with pullups
    #endif
  #endif

  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  GUI.tuneToStation(_lastStation);
  xSemaphoreGiveRecursive(lvgl_mux);
  audioConnecttohost(tinyStations[_lastStation].URL.c_str());

  createVU_Task();
}

#ifdef USE_ENCODER_VOLUME
void rotary_volume_loop()
{
  // dont do anything unless value changed
  if (volumeEncoder.encoderChanged())
  {
    uint8_t v = (uint8_t)volumeEncoder.readEncoder();
    if (v <= _maxVolume)
    {
      _lastVolume = v;
      audioSetVolume(_lastVolume);
      xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      GUI.setVolumeIndicator(_lastVolume);
      xSemaphoreGiveRecursive(lvgl_mux);
      savePrefs();
    }
  }
  // handle_rotary_button();
}
#endif

#ifdef USE_ENCODER_TUNE
// EXPERIEMANTAL !!
void rotary_tune_loop()
{
  // dont do anything unless value changed
  if (tuneEncoder.encoderChanged())
  {
    uint32_t v = tuneEncoder.readEncoder();
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.setTuneIndicator(v);
    xSemaphoreGiveRecursive(lvgl_mux);
    int8_t idx = GUI.getStationIndexUnderIndicator(v);
    if (idx >= 0)
    {
      if (idx != _lastStation)
      { // NEW STATION
        LV_LOG_USER("NEW STATION !!!");
        _lastStation = idx;
        audioConnecttohost(tinyStations[_lastStation].URL.c_str());
        audioSetVolume(_lastVolume);
        savePrefs();
        _isPaused = false;
      }
      else
      { // SAME STATION(AGAIN)
        LV_LOG_USER(" * SAME STATION *");
        xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
        GUI.setStationPlaying(GUI.last_station.c_str());
        GUI.setTitlePlaying(GUI.last_title.c_str());
        xSemaphoreGiveRecursive(lvgl_mux);
        audioSetVolume(_lastVolume);
        savePrefs();
        _isPaused = false;

        //  if(_isPaused) {_isPaused = audioPauseResume(); }
      }
    }
    else
    { // NO STATION
      LV_LOG_USER("** NO STATION **");
      // if(!_isPaused) {_isPaused = audioPauseResume(); }
      //  _isPaused = audioPauseResume();
      xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
      GUI.setStationPlaying("", false); // DONN'T STORE
      GUI.setTitlePlaying("", false);
      xSemaphoreGiveRecursive(lvgl_mux);
      audioSetVolume(0); // MUTE
      _isPaused = true;
    }
  }
  // handle_rotary_button();
}
#endif

// SparkOfCode: set volume with potentiometer
#ifdef USE_POT_VOLUME
void pot_volume_loop()
{
  // dont do anything unless value changed - use moving average to prevent erratic jumping between adjacent steps
  volPotValueNew = (volPotValueNew * 19 + analogRead(POT_VOL_PIN)) / 20; // 0..4095
  if (abs(volPotValueNew - volPotValue) > (4096 / VOLUME_STEPS))         //
  {
    volPotValue = volPotValueNew;
    _lastVolume = map(volPotValue, 0, 4095, 0, VOLUME_STEPS - 1);
    audioSetVolume(_lastVolume);
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.setVolumeIndicator(_lastVolume);
    xSemaphoreGiveRecursive(lvgl_mux);
  }
}
#endif

void loop()
{
  ESPAsync_WiFiManager->run();

  if (!_drdStopped)
  {

    if (!drd->waitingForDRD())
    {
      // FALSE IF ENDED !
      _drdStopped = true;
      LV_LOG_USER("DRD STOPPED");
      GUI.updateDRDindicator(true);
    }
  }

  uint32_t _codec = 0;

  if (audioIsRunning)
  {

    _vum = audioGetVUlevel();

#ifdef USE_ENCODER_VOLUME
    rotary_volume_loop();
#endif

#ifdef USE_ENCODER_TUNE
    rotary_tune_loop();
#endif

// SparkOfCode TUNE_ENCODER_SM
#ifdef USE_ENCODER_TUNE_SM
    int charEncoderChanged;
    charEncoderChanged = encoderChanged();
    if (charEncoderChanged == 1) // clockwise
    {
      gui_station_next();
    }
    if (charEncoderChanged == -1) // counter-clockwise
    {
      gui_station_prev();
    }
#endif

// SparkOfCode: Volume Potentiometer
#ifdef USE_POT_VOLUME
    pot_volume_loop();
#endif
  }

}

// ************** WIFI_MANAGER CALLBACKS *************************************
void wifimanager_message(const char *info)
{

  if (tempLabel)
  {
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    lv_label_set_text(tempLabel, info);
    xSemaphoreGiveRecursive(lvgl_mux);
  }
}

void wifimanager_drdStopped(const char *info)
{

  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  GUI.updateDRDindicator(true);
  xSemaphoreGiveRecursive(lvgl_mux);
}

// ************** AUDIO CALLBACKS *************************************
const char *_codecname[10] = {"unknown", "WAV", "MP3", "AAC", "M4A", "FLAC", "AACP", "OPUS", "OGG", "VORBIS"};

void audio_info(const char *info)
{
  //  LV_LOG_USER(info);
}
void audio_showstation(const char *info)
{

  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
  GUI.setStationPlaying(info);

  xSemaphoreGiveRecursive(lvgl_mux);
}

void audio_showstreamtitle(const char *info)
{
  xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);

  GUI.setTitlePlaying(info);
  xSemaphoreGiveRecursive(lvgl_mux);
}

// ************** BUTTON (GUI) CALLBACKS *************************************
void gui_volume_up()
{
  if (_lastVolume < _maxVolume)
  {
    _lastVolume++;
    audioSetVolume(_lastVolume);
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.setVolumeIndicator(_lastVolume);
    xSemaphoreGiveRecursive(lvgl_mux);
    savePrefs();
  }
}

void gui_volume_down()
{
  if (_lastVolume > 0)
  {
    _lastVolume--;
    audioSetVolume(_lastVolume);
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.setVolumeIndicator(_lastVolume);
    xSemaphoreGiveRecursive(lvgl_mux);

    savePrefs();
  }
}

void gui_station_next()
{
  if (_lastStation < _num_stations - 1)
  {
    _lastStation++;
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.tuneToStation(_lastStation);
    xSemaphoreGiveRecursive(lvgl_mux);

    audioConnecttohost(tinyStations[_lastStation].URL.c_str());
    savePrefs();
  }
}

void gui_station_prev()
{
  if (_lastStation > 0)
  {
    _lastStation--;

    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    GUI.tuneToStation(_lastStation);
    xSemaphoreGiveRecursive(lvgl_mux);

    audioConnecttohost(tinyStations[_lastStation].URL.c_str());
    savePrefs();
  }
}