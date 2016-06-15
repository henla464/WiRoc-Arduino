#include "SystemInfoMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"
#include "Util.h"

SystemInfoMenu::SystemInfoMenu() : AbstractState()
{
  
}

void SystemInfoMenu::Init()
{
  AbstractState::Init();
  lastUpdateMillis = millis();
  noOfRightClicks = 0;
  
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("SW version: v");
  lcd.print(SOFTWARE_VERSION);
  lcd.setCursor(0,1);
  lcd.print("HW version: v");
  lcd.print(Settings::GetHWVersion());
}


AbstractState* SystemInfoMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;  
}

AbstractState* SystemInfoMenu::Left()
{
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}

AbstractState* SystemInfoMenu::Right()
{
  noOfRightClicks++;
  if (noOfRightClicks >= 3) {
    return (AbstractState*)&LCDStates::TheHiddenMenu;
  }
  return NULL;
}
