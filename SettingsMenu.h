#ifndef __SettingsMenu_H__
#define __SettingsMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"

class SettingsMenu : public AbstractState 
{
  public:
    SettingsMenu();
    virtual void Init() override;
    virtual AbstractState* Select() override;
    virtual AbstractState* Tick() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
    virtual AbstractState* Right() override;
  private:
    void printMenu();
    void printArrow();
    AbstractState* GetSelected();
    uint8_t cursorIndex;
    uint8_t firstRowIndex;
    static char SettingMenuItems[][15];
};

#endif // __SettingsMenu_H__
