
/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

  *****************************************************************************************************************************/
 
#ifndef _TINY_DEFS_H_
#define _TINY_DEFS_H_


#include <lvgl.h>


//COMMENT NEXT LINE, IF YOU WANT TO USE "EM11" FOR VU-METER
#define USE_VU_METER

//UNCOMMENT NEXT LINE, IF YOU WANT TO USE ENCODER FOR VOLUME
//#define USE_ENCODER_VOLUME

//UNCOMMENT NEXT LINE, IF YOU WANT TO USE ENCODER FOR TUNING
//#define USE_ENCODER_TUNE 

//SparkOfCode
//Uncomment to use encoder for volume implemented as state machine
#define USE_ENCODER_TUNE_SM
//Uncomment if your encoder has no pullup resistors and you need the internal GPIO pullups of ESP32
#define USE_ENC_TUNE_INTERNAL_PULLUP

//SparkOfCode
//Uncomment next line if you want to use a potentiometer on ADC for volume control
#define USE_POT_VOLUME

//ENCODER PINS
#ifdef USE_ENCODER_VOLUME
//PINS
  #define ENC_VOUME_BUTTON_PIN    10
  #define ENC_VOLUME_A_PIN        11
  #define ENC_VOLUME_B_PIN        12
#endif

#ifdef USE_ENCODER_TUNE
//PINS
  #define ENC_TUNE_BUTTON_PIN     42
  #define ENC_TUNE_A_PIN          14
  #define ENC_TUNE_B_PIN          21
#endif

//SparkOfCode
#ifdef USE_ENCODER_TUNE_SM
//PINS
  #define ENC_TUNE_BUTTON_PIN     42 // not used, should be commented out eventually to avoid side effects
  #define ENC_TUNE_A_PIN          14
  #define ENC_TUNE_B_PIN          21
#endif

#ifdef USE_POT_VOLUME
  #define POT_VOL_PIN     10 // GPIO10 is ADC1_CH9 (pin 3 of Extended IO Interface)
#endif

#define HOST_NAME   "TinyRadio" //FOR CONNECT VIA BROWSER -> use TinyRadio.local !

//********************************************************************************
//*********  DON'T CHANGE ANYTHING BELOW THIS LINE !! ****************************
//********************************************************************************


//BUTTONMATRIX SYMBOLS
//NO ENCODER
#if !defined(USE_ENCODER_VOLUME) && !defined(USE_ENCODER_TUNE)
  static const char * btnm_map[] = {LV_SYMBOL_VOLUME_MID, LV_SYMBOL_PREV, LV_SYMBOL_NEXT, LV_SYMBOL_VOLUME_MAX ,""};
  #define VOLUME_STEPS 20
  #define TUNE_STEPS    4
#endif

//ONLY TUNE ENCODER
#if !defined(USE_ENCODER_VOLUME) && defined(USE_ENCODER_TUNE)
  static const char * btnm_map[] = {LV_SYMBOL_VOLUME_MID, LV_SYMBOL_VOLUME_MAX ,""};
#endif

//ONLY VOLUME ENCODER
#if defined(USE_ENCODER_VOLUME) && !defined(USE_ENCODER_TUNE)
  static const char * btnm_map[] = {LV_SYMBOL_PREV, LV_SYMBOL_NEXT,""};
  #define VOLUME_STEPS 50
  #define TUNE_STEPS    4
#endif

// VOLUME AND TUNE ENCODER
#if defined(USE_ENCODER_VOLUME) && defined(USE_ENCODER_TUNE)
  static const char * btnm_map[] = {""};  //DUMMY
  #define NO_BUTTONS
  #define VOLUME_STEPS 50
  #define TUNE_STEPS    4
#endif

// SparkOfCode: Volume Potentiometer
#ifdef USE_POT_VOLUME
  #define VOLUME_STEPS 119 // 100 or 120 does not use the full display width - bug in RetroGUI?
#endif

  #define INDICATOR_MOVE_TIME 2000 // 2 sec. (2000 mS) from left to right

  //AUDIO
  #define MAX_STATIONS 19  //DO NOT INCREASE !!! (IT LOOKS UGLY ...)

  //LVGL
  #define LVGL_TASK_DELAY_MS   10

  #define LVGL_TASK_CORE 1
  #define LVGL_TASK_PRIO 5



  // //PREFERENCES
  #define PREFS_NAME    "TinyRadio"
  #define LAST_STATION  "rec_lastStation"
  #define LAST_VOLUME   "rec_lastVolume"

  // // GUI
  #define RADIUS 2
  #define BORDER_WIDTH 1
  #define POINTER_WIDTH 8  

  #define X_INC  20
  #define Y_INC  27
  #define Y_INC_RAMS  20  //27

  #define LABEL_WIDTH 6 * X_INC
  #define MARKER_HEIGHT 6
  #define MARKER_WIDTH X_INC


#endif //_TINY_DEFS_H_