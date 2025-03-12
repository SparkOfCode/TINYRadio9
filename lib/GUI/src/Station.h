/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

*****************************************************************************************************************************/
#pragma once

#include "Arduino.h"

class Station
{
public:
    Station(); // Default Constructor
    Station(String ShortName, String URL); // Constructor
    //
private:
        // from typedef struct typeStructTinyStation
        String _ShortName;
        String _URL;
        uint8_t _index;
        uint32_t _startX; // COORDINATES FOR GUI (IF NEEDED)
        uint32_t _midX;
        uint32_t _endX;
};