/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

  *****************************************************************************************************************************/

#ifndef _STATIONS_H_
#define _STATIONS_H_

#include <Arduino.h>
#include <Array.h>
#include <ArduinoJson.h>

#define MAX_STATIONS 19 // 19 stations per page

typedef struct
{
  String ShortName;
  String URL;
  uint8_t index;
  uint32_t startX; // COORDINATES FOR GUI (IF NEEDED)
  uint32_t midX;
  uint32_t endX;
} typeStructTinyStation;

// TYPE-DEF
typedef Array<typeStructTinyStation, MAX_STATIONS> typeArrStations;

class clTinyStations
{

public:
  typeArrStations arrStations; // array[19] of typeStructTinyStation
  uint8_t lastStation;         // last station of page

  clTinyStations() // Constructor
  {
    lastStation;
  }

  typeArrStations *getArrStations()
  {
    return (&arrStations);
  }

  void loadFromJson(uint16_t Page, String *json)
  {
    Serial.println(Page);
    Serial.println(*json);
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, (*json).c_str());

    if (error)
    {
      LV_LOG_USER("deserializeJson() failed: ");
      LV_LOG_USER(error.c_str());
      return;
    }

    arrStations.clear();

    uint8_t idx = 0;
    for (JsonObject station : doc["stations"].as<JsonArray>())
    {
      typeStructTinyStation _station;

      _station.ShortName = station["shortName"].as<String>();
      _station.URL = station["StreamURL"].as<String>();
      _station.index = idx;
      if (station["Page"] == Page)
      {
        arrStations.push_back(_station);
      };
      idx++;
    }
  }
};

#endif