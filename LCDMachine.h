#ifndef __LCDMachine_H__
#define __LCDMachine_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>   // include LCD library
//#include "SettingsMenu.h"
//#include "MainMenu.h"
#include "LCDStates.h"

/*--------------------------------------------------------------------------------------
  Defines
--------------------------------------------------------------------------------------*/

// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            131  // up
#define DOWN_10BIT_ADC          307  // down
#define LEFT_10BIT_ADC          480  // left
#define SELECT_10BIT_ADC        720  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 

class LCDMachine
{
  public:
    static AbstractState* CurrentMenu;
    static void Init(SIStation *station, Radio *outboundRadio, uint8_t buttonPin, uint8_t backlightPin);
    static void Tick();
    static SIStation *Station;
    static Radio *OutboundRadio;
    static uint8_t ButtonPin;
    static uint8_t BacklightPin;
  private:
    static uint8_t ReadButtons();
    static void TurnBacklightOn();
    static void TurnBacklightOff();

    static uint8_t buttonJustPressed;
    static uint8_t buttonJustReleased;
    static uint8_t buttonWas;
    static uint8_t buttonReleased;
    static uint32_t millisLastTick;
    static uint32_t millisLastButtonPress;
};

#endif // __LCDMachine_H__
