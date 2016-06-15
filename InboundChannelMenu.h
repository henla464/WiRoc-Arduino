#ifndef __InboundChannelMenu_H__
#define __InboundChannelMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class InboundChannelMenu : public AbstractState 
{
  public:
    InboundChannelMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
  private:
    uint8_t channelIndex;
    void printChannel();
    void initChannelIndex();
};

#endif // __InboundChannelMenu_H__
