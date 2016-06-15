#ifndef __SystemInfoMenu_H__
#define __SystemInfoMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include "Settings.h"

class SystemInfoMenu : public AbstractState 
{
  public:
    SystemInfoMenu();
    virtual void Init() override;
    virtual AbstractState* Left() override;
    virtual AbstractState* Right() override;
    virtual AbstractState* Tick() override;
  private:
    uint32_t lastUpdateMillis;
    uint8_t noOfRightClicks;
};

#endif // __SystemInfoMenu_H__
