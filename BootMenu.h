#ifndef __BootMenu_H__
#define __BootMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "LCDMachine.h"

enum boot_states_t {NOT_INITIALIZED, RADIO, RADIO_ERROR, RADIO_RECONFIGURED, RADIO_ALREADY_CONFIGURED,                
                SD_CARD, SD_CARD_FAILED, SD_CARD_DB_OPENED, SD_CARD_DB_CREATED, SD_CARD_LOG_OPENED, INITIALIZED,
                CONTINUE_AFTER_ERROR};

class BootMenu : public AbstractState 
{
  public:
    boot_states_t BootState;
    bool IsConfiguringInboundRadio;
    String LogFileName;
    BootMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    //virtual AbstractState* Left() override;
  private:
    void printMenu();
    void printRow1(String txt);
    void printRow2(String txt);
};

#endif // __BootMenu_H__
