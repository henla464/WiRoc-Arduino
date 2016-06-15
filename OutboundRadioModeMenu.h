#ifndef __OutboundRadioModeMenu_H__
#define __OutboundRadioModeMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class OutboundRadioModeMenu : public AbstractState 
{
  public:
    OutboundRadioModeMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
  private:
    uint8_t radioModeIndex;
    void printMenu();
    void initRadioModeIndex();
};

#endif // __OutboundRadioModeMenu_H__
