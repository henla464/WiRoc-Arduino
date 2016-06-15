#include "BatteryMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

BatteryMenu::BatteryMenu() : AbstractState()
{
  
}

void BatteryMenu::Init()
{
  AbstractState::Init();
  lastUpdateMillis = millis();
  
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Percentage: ");
 
  lcd.setCursor(0,1);
  lcd.print("Voltage: ");
 
  printBatteryNumbers();
}

void BatteryMenu::printBatteryNumbers()
{
  lcd.setCursor(12,0);
  uint8_t p = Battery::GetBatteryPercentage();
  if (p < 100)
  {
    lcd.print(" ");
    if (p < 10)
    {
      lcd.print(" ");
    }
  }
  lcd.print(p);
  lcd.print("%");
 
  lcd.setCursor(11,1);
  lcd.print(Battery::GetBatteryVoltage(),2);
  lcd.print("V");
}

AbstractState* BatteryMenu::Tick()
{
   if (millis()-millisAtInit > 60000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   if (millis()-lastUpdateMillis > 1000)
   {
      lastUpdateMillis = millis();
      printBatteryNumbers();
   }
   return NULL;  
}

AbstractState* BatteryMenu::Left()
{
    return (AbstractState*)&LCDStates::TheSettingsMenu;
}
