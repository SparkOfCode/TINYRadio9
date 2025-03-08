/****************************************************************************************************************************
  This is for  WT32_SC01_PLUS boards only !

  Licensed under MIT license

  by DrNeurosurg 2024

  *****************************************************************************************************************************/

#pragma once

#include "GUIClass.h"
#include "WT32_SC01_PLUS.h"

// CALLBACKS
extern __attribute__((weak)) void gui_tuneToURL(String URL);
extern __attribute__((weak)) void gui_setVolume(uint8_t newVolume);

extern __attribute__((weak)) void gui_volume_up();
extern __attribute__((weak)) void gui_volume_down();
extern __attribute__((weak)) void gui_station_next();
extern __attribute__((weak)) void gui_station_prev();

extern __attribute__((weak)) void gui_page_next();
extern __attribute__((weak)) void gui_page_prev();
extern __attribute__((weak)) void gui_setPage(int Page);
//extern __attribute__((weak)) void gui_scrollPanel(int Page);

class RetroGUI : public GuiClass
{

public:
    String last_title = "";
    String last_station = "";

    void update(String json);
    void begin(lv_obj_t *parent, String json);
    void setPage(uint8_t* page);
    void setStations(typeArrStations* stations);
    void setStationPlaying(const char *station, bool store = true);
    void setTitlePlaying(const char *title, bool store = true);
    void setVolumeIndicatorMax(uint8_t maxValue);
    static void anim_x_cb(void *var, int32_t v);
    void setVolumeIndicator(uint8_t value);
    bool inRange(int val, int minimum, int maximum);
    int8_t getStationIndexUnderIndicator(uint32_t indicatorPos);
    void setTuneIndicator(uint32_t newPosition);
    void tuneToStation(uint8_t station_id);
    void setVUMeterValue(uint16_t value);
    void updateDRDindicator(bool drdStopped);
    uint32_t getStationMidX(uint8_t stationIndex);
    void panelScroll(int Page);
    void swapPage(uint8_t Page, uint8_t station, typeArrStations* arrStations);

/************************************************************
/******************* private ********************************
/***********************************************************/

private:


    typeArrStations _stations;

    uint8_t _maxVolume;
    uint8_t _volumePercent; // FOR BAR
    uint8_t _volume;
    uint8_t _currentPage;
    uint32_t _lastStationMid = 0;

    uint32_t _pointer_width = 8;
    uint32_t _border_width = 2;
    uint32_t _volume_width;

    lv_obj_t *cont;              // CONTAINER
    lv_obj_t *cont_mid;              // CONTAINER for station list
    lv_obj_t *station_indicator; // SKALENZEIGER
    lv_obj_t *volume_indicator;  // VOLUME
    lv_obj_t *tl;                // TOP LEFT
    lv_obj_t *tr;                // TOP RIGHT
    lv_obj_t * mid_parent; //MIDDLE parent object (for scroll)
    lv_obj_t * panel;
    lv_obj_t * mid; //MIDDLE
    lv_obj_t * mid2; //MIDDLE 2
    lv_obj_t * mid3; //MIDDLE 2
    lv_obj_t * mid4; //MIDDLE 2
    lv_obj_t *bot_upper; // BOTTOM
    lv_obj_t *bot_lower; // BOTTOM
    lv_obj_t *stationList;
    lv_obj_t *scale_bot;
    lv_obj_t *station_playing;
    lv_obj_t *title_playing;
    lv_obj_t *buttons;
    lv_obj_t *volume_scale;

    lv_obj_t *btn_volUp;
    lv_obj_t *btn_volDown;
    lv_obj_t *btn_tuneNext;
    lv_obj_t *btn_tunePrev;

    // VU-METER
    lv_obj_t *em11;

    lv_obj_t *vuMeter;
    lv_obj_t *needle;

    // STYLES
    lv_style_t panel_style;
    lv_style_t cont_style;
    lv_style_t tuning_style;
    lv_style_t volume_style;
    lv_style_t part_style;
    lv_style_t playing_style;
    lv_style_t playing_style_inv;
    lv_style_t button_style;
    lv_style_t button_style_pressed;
    lv_style_t scale_style;
    lv_style_t vuMeter_style;

    String dummies[20] = {
        "NÜRNBERG",
        "NDR-WDR",
        "FREDERIKST.",
        "FLENSBURG",
        "MALMÖ",
        "SÜDWESTFUNK",
        "VATIKAN",
        "BRÜSSEL",
        "MÜNSTER",
        "BAYR.-RDFK.",
        "MTE CARLO",
        "LUXEMBURG",
        "SAARBRÜCKEN",
        "MERGENTHEIM",
        "SCHWEIZ",
        "BREMEN",
        "STAVANGER",
        "NORDEN BBC",
        "STRASSBURG",
        "LILLE"};

    void createStyles();
    lv_obj_t *createPart(lv_obj_t *parent, bool border);
    lv_obj_t *createButtons(lv_obj_t *parent);
    lv_obj_t *createPlayingLabel(lv_obj_t *parent, const lv_font_t *font, bool top, bool inverse);
    lv_obj_t *createStationIndicator(lv_obj_t *parent);
    lv_obj_t *createVolumeIndicator(lv_obj_t *parent);
    lv_obj_t *createStation(lv_obj_t *parent, const lv_font_t *font, bool inverse);
    void createStationList(lv_obj_t *parent);
    lv_obj_t *createVolumeScale(lv_obj_t *parent, const lv_font_t *font, int min, int max);
    void create_vuMeter(lv_obj_t *parent);
    static void _event_handler_buttonmatrix(lv_event_t *e);
          
};