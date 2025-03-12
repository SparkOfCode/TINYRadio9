/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

*****************************************************************************************************************************/

#include "Station.h"

Station::Station() // Default Constructor
{
};
Station::Station(String ShortName, String URL /*more?*/) // Constructor
{
  _ShortName = ShortName;
  _URL = URL;
};
