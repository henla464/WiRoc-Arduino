#include "HiddenMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

HiddenMenu::HiddenMenu() : AbstractState()
{
  
}

void HiddenMenu::Init()
{
  AbstractState::Init();
  lastUpdateMillis = millis();
  hwVersion = Settings::GetHWVersion();
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("HW Version: v");
  Print();
}

void HiddenMenu::Print()
{
  lcd.setCursor(13,0);
  lcd.print(hwVersion);
  lcd.print("  ");
}

AbstractState* HiddenMenu::Tick()
{
  if (millis()-millisAtInit > 60000)
  {
    return (AbstractState*)&LCDStates::TheMainMenu;
  }
  return NULL;
}

AbstractState* HiddenMenu::Left()
{
  return (AbstractState*)&LCDStates::TheSystemInfoMenu;
}

AbstractState* HiddenMenu::Up()
{
  hwVersion++;
  Print();
  return NULL;
}

AbstractState* HiddenMenu::Down()
{
  hwVersion--;
  Print();
  return NULL;
}

AbstractState* HiddenMenu::Select()
{
  Settings::SetHWVersion(hwVersion);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}
